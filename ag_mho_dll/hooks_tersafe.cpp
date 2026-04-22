#include "hooks_tersafe.h"
#include "log.h"
#include "hook_table.h"
#include "str_util.h"
#include "win_util.h"

#include <windows.h>
#include <psapi.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>

/* ============================================================================
 * TerSafe CreateObj-vtable full-observation hooks.
 *
 * Every one of the six vtable slots is routed through a per-slot naked byte
 * trampoline that captures the full calling context on entry and exit, then
 * forwards to real TerSafe. The trampoline body is byte-identical in its
 * critical path (entry through the CALL of real_vt_N) to the pattern proven
 * safe for vt_1 — i.e. the TerSafe tamper check (which we empirically showed
 * is sensitive to a C-compiled wrapper's extra pushes) does not see anything
 * different from a direct CryGame→TerSafe call.
 *
 * Info logged per call, covering:
 *   (1) exact stack-arg count — measured via ESP delta across the real call
 *   (2) arg value (raw 32-bit dword) for up to 8 stack slots
 *   (3) arg "type" hint — any value that maps to a valid readable address is
 *       reported as a pointer and its pointed-to memory is snapshot+diffed;
 *       otherwise reported as a raw integer.
 *   (4) this pointer (ECX), EDX, caller return address, thread id
 *   (5) return value (EAX) with both unsigned hex and signed int printing
 *   (6) hex dump of `this` (256 bytes) and of every pointer-arg (64 bytes
 *       each) BEFORE the call
 *   (7) hex dump + dword diff of `this` and every pointer-arg AFTER the call.
 *
 * Stack layout at tramp entry (just after CryGame's `call [vtable+slot*4]`):
 *     [esp+0x00] = R_caller (pushed by the CALL)
 *     [esp+0x04..] = 0 or more pushed args  (up to 8 captured)
 *     ECX = this (for __thiscall), EDX = scratch (for __thiscall)
 *
 * Per-slot tramp pseudo-asm:
 *     pop   eax;  mov [s.caller],  eax
 *     mov   [s.this_ptr], ecx
 *     mov   [s.edx], edx
 *     ; copy [esp+0..+28] into s.args[0..7]
 *     pushad / pushfd
 *     push  <slot>
 *     call  observer_before  (cdecl)
 *     add   esp, 4
 *     popfd / popad
 *     mov   [s.esp_before], esp
 *     call  [g_real_vt[slot]]     ; same critical-path shape as iteration 7
 *     mov   [s.esp_after], esp
 *     mov   [s.retval],   eax
 *     pushad / pushfd
 *     push  <slot>
 *     call  observer_after   (cdecl)
 *     add   esp, 4
 *     popfd / popad               ; popad restores eax = retval (pushad saved it)
 *     push  [s.caller]
 *     ret
 *
 * ESP delta: esp_after − esp_before = the number of bytes the callee cleaned
 * via its `ret K` epilogue.  For __thiscall / __stdcall (callee cleanup) this
 * directly gives us K = argcount × 4.
 * ========================================================================== */

#define SNAP_SIZE   0x100       /* 256 bytes — documented TsCreateObj size */
#define ARG_SNAP    0x40        /* 64 bytes per pointer-looking arg */
#define MAX_ARGS    8           /* capture up to 8 stack dwords */
#define NUM_SLOTS   6
#define NUM_SUBS    2           /* sub_obj_0c and sub_obj_10 */
#define SUB_SLOTS   16          /* max sub-vtable entries we hook */

static const char *VT_NAMES[NUM_SLOTS] = {
    "Init", "RecvData", "ProcessPacket", "QueryState", "Tick", "Release"
};

/* Captured at tramp entry/exit — one per slot, single-threaded. */
struct SlotState {
    /* Entry-time captures: */
    uint32_t caller;
    uint32_t this_ptr;
    uint32_t edx;
    uint32_t args[MAX_ARGS];

    /* Exit-time captures: */
    uint32_t retval;
    uint32_t esp_before;
    uint32_t esp_after;

    /* Snapshot buffers populated by observer_before, diffed by observer_after: */
    uint8_t this_snapshot[SNAP_SIZE];
    bool    this_valid;
    uint8_t edx_snapshot[ARG_SNAP];         /* for __fastcall-style EDX-as-arg */
    bool    edx_valid;
    uint8_t arg_snapshots[MAX_ARGS][ARG_SNAP];
    bool    arg_valid[MAX_ARGS];
};
static SlotState g_slot[NUM_SLOTS];
static SlotState g_sub_slot[NUM_SUBS][SUB_SLOTS];

static void *g_real_vt[NUM_SLOTS];
static void *g_real_sub[NUM_SUBS][SUB_SLOTS];
static void *g_tersafe_obj = nullptr;

/* ---- helpers -------------------------------------------------------------- */

static bool safe_copy(void *dst, const void *src, size_t n) {
    if (!src) return false;
    if (IsBadReadPtr(src, n)) return false;
    memcpy(dst, src, n);
    return true;
}

/* Loose heuristic: is this value plausibly a readable user-space pointer? */
static bool looks_like_ptr(uint32_t v) {
    return v >= 0x00010000u && v < 0x80000000u;
}

static void hex_dump(const char *label, const uint8_t *buf, int size) {
    log("  %s FULL DUMP (0x%X bytes):\n", label, size);
    for (int i = 0; i < size; i += 16) {
        const uint8_t *p = buf + i;
        bool zero = true;
        for (int j = 0; j < 16 && i + j < size; j++) {
            if (p[j]) { zero = false; break; }
        }
        if (zero) continue;
        log("    +%03X: %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n",
            i,
            p[0], p[1], p[2],  p[3],  p[4],  p[5],  p[6],  p[7],
            p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
    }
}

static void diff_mem(const char *label, const uint8_t *before, const uint8_t *after, int size) {
    const uint32_t *b = (const uint32_t *)before;
    const uint32_t *a = (const uint32_t *)after;
    int dwords = size / 4;
    int changes = 0;
    for (int i = 0; i < dwords; i++) {
        if (b[i] != a[i]) {
            log("    %s[+0x%02X] CHANGED: 0x%08X -> 0x%08X\n", label, i * 4, b[i], a[i]);
            changes++;
        }
    }
    if (changes == 0) log("    %s: (no dword changes)\n", label);
}

/* ---- observers ------------------------------------------------------------ */

extern "C" void __cdecl tersafe_observer_before(int slot) {
    if (slot < 0 || slot >= NUM_SLOTS) return;
    SlotState &s = g_slot[slot];
    const char *name = VT_NAMES[slot];

    log("[TS:vt%d:%s] ENTRY this=0x%08X edx=0x%08X caller=0x%08X tid=%lu\n",
        slot, name, s.this_ptr, s.edx, s.caller, GetCurrentThreadId());
    log("  stack[0..7] = %08X %08X %08X %08X %08X %08X %08X %08X\n",
        s.args[0], s.args[1], s.args[2], s.args[3],
        s.args[4], s.args[5], s.args[6], s.args[7]);

    /* Snapshot `this` (256 bytes). */
    s.this_valid = safe_copy(s.this_snapshot, (void *)s.this_ptr, SNAP_SIZE);
    if (s.this_valid) hex_dump("this_before", s.this_snapshot, SNAP_SIZE);
    else              log("  (this snapshot failed)\n");

    /* Snapshot EDX target — covers __fastcall-style second-register args
     * (e.g. vt_1 passes its anti-cheat dataBuffer in EDX, not on the stack). */
    s.edx_valid = false;
    if (looks_like_ptr(s.edx)) {
        if (safe_copy(s.edx_snapshot, (void *)s.edx, ARG_SNAP)) {
            s.edx_valid = true;
            char label[32];
            snprintf(label, sizeof(label), "edx_before(0x%08X)", s.edx);
            hex_dump(label, s.edx_snapshot, ARG_SNAP);
        }
    }

    /* Snapshot every stack slot that looks like a valid readable pointer. */
    for (int i = 0; i < MAX_ARGS; i++) {
        s.arg_valid[i] = false;
        if (!looks_like_ptr(s.args[i])) continue;
        if (safe_copy(s.arg_snapshots[i], (void *)s.args[i], ARG_SNAP)) {
            s.arg_valid[i] = true;
            char label[32];
            snprintf(label, sizeof(label), "arg%d_before(0x%08X)", i, s.args[i]);
            hex_dump(label, s.arg_snapshots[i], ARG_SNAP);
        }
    }
}

extern "C" void __cdecl tersafe_observer_after(int slot) {
    if (slot < 0 || slot >= NUM_SLOTS) return;
    SlotState &s = g_slot[slot];
    const char *name = VT_NAMES[slot];

    int32_t delta = (int32_t)(s.esp_after - s.esp_before);
    int     n_args_inferred = (delta >= 0) ? (delta / 4) : -1;

    log("[TS:vt%d:%s] RETURN retval=0x%08X (%d)  esp_delta=%d bytes => %d stack arg(s) [thiscall/stdcall]\n",
        slot, name, s.retval, (int32_t)s.retval, delta, n_args_inferred);

    /* Diff `this`. */
    uint8_t this_after[SNAP_SIZE];
    if (safe_copy(this_after, (void *)s.this_ptr, SNAP_SIZE)) {
        if (s.this_valid) diff_mem("this", s.this_snapshot, this_after, SNAP_SIZE);
        hex_dump("this_after", this_after, SNAP_SIZE);
    } else {
        log("  (this_after snapshot failed)\n");
    }

    /* Diff EDX target (covers __fastcall-style EDX-as-arg). */
    if (s.edx_valid) {
        uint8_t edx_after[ARG_SNAP];
        if (safe_copy(edx_after, (void *)s.edx, ARG_SNAP)) {
            char label[32];
            snprintf(label, sizeof(label), "edx(0x%08X)", s.edx);
            diff_mem(label, s.edx_snapshot, edx_after, ARG_SNAP);
        }
    }

    /* Diff each pointer arg we snapshotted earlier. */
    for (int i = 0; i < MAX_ARGS; i++) {
        if (!s.arg_valid[i]) continue;
        uint8_t arg_after[ARG_SNAP];
        if (safe_copy(arg_after, (void *)s.args[i], ARG_SNAP)) {
            char label[32];
            snprintf(label, sizeof(label), "arg%d(0x%08X)", i, s.args[i]);
            diff_mem(label, s.arg_snapshots[i], arg_after, ARG_SNAP);
        }
    }
}

/* ---- sub-vtable observers (sub_obj_0c and sub_obj_10) --------------------
 *
 * idx encodes (sub_idx << 8) | slot so the naked trampoline can pass a
 * single int to the cdecl observer.
 * -------------------------------------------------------------------------- */

extern "C" void __cdecl tersafe_sub_observer_before(int idx) {
    int sub_idx = (idx >> 8) & 0xFF;
    int slot    = idx & 0xFF;
    if (sub_idx < 0 || sub_idx >= NUM_SUBS) return;
    if (slot < 0 || slot >= SUB_SLOTS) return;
    SlotState &s = g_sub_slot[sub_idx][slot];

    log("[TS:sub%d_vt%d] ENTRY this=0x%08X edx=0x%08X caller=0x%08X tid=%lu\n",
        sub_idx, slot, s.this_ptr, s.edx, s.caller, GetCurrentThreadId());
    log("  stack[0..7] = %08X %08X %08X %08X %08X %08X %08X %08X\n",
        s.args[0], s.args[1], s.args[2], s.args[3],
        s.args[4], s.args[5], s.args[6], s.args[7]);

    s.this_valid = safe_copy(s.this_snapshot, (void *)s.this_ptr, SNAP_SIZE);
    if (s.this_valid) hex_dump("this_before", s.this_snapshot, SNAP_SIZE);

    s.edx_valid = false;
    if (looks_like_ptr(s.edx)) {
        if (safe_copy(s.edx_snapshot, (void *)s.edx, ARG_SNAP)) {
            s.edx_valid = true;
            char label[32];
            snprintf(label, sizeof(label), "edx_before(0x%08X)", s.edx);
            hex_dump(label, s.edx_snapshot, ARG_SNAP);
        }
    }

    for (int i = 0; i < MAX_ARGS; i++) {
        s.arg_valid[i] = false;
        if (!looks_like_ptr(s.args[i])) continue;
        if (safe_copy(s.arg_snapshots[i], (void *)s.args[i], ARG_SNAP)) {
            s.arg_valid[i] = true;
            char label[32];
            snprintf(label, sizeof(label), "arg%d_before(0x%08X)", i, s.args[i]);
            hex_dump(label, s.arg_snapshots[i], ARG_SNAP);
        }
    }
}

extern "C" void __cdecl tersafe_sub_observer_after(int idx) {
    int sub_idx = (idx >> 8) & 0xFF;
    int slot    = idx & 0xFF;
    if (sub_idx < 0 || sub_idx >= NUM_SUBS) return;
    if (slot < 0 || slot >= SUB_SLOTS) return;
    SlotState &s = g_sub_slot[sub_idx][slot];

    int32_t delta = (int32_t)(s.esp_after - s.esp_before);
    int     n_args_inferred = (delta >= 0) ? (delta / 4) : -1;

    log("[TS:sub%d_vt%d] RETURN retval=0x%08X (%d)  esp_delta=%d bytes => %d stack arg(s)\n",
        sub_idx, slot, s.retval, (int32_t)s.retval, delta, n_args_inferred);

    uint8_t this_after[SNAP_SIZE];
    if (safe_copy(this_after, (void *)s.this_ptr, SNAP_SIZE)) {
        if (s.this_valid) diff_mem("this", s.this_snapshot, this_after, SNAP_SIZE);
        hex_dump("this_after", this_after, SNAP_SIZE);
    }

    if (s.edx_valid) {
        uint8_t edx_after[ARG_SNAP];
        if (safe_copy(edx_after, (void *)s.edx, ARG_SNAP)) {
            char label[32];
            snprintf(label, sizeof(label), "edx(0x%08X)", s.edx);
            diff_mem(label, s.edx_snapshot, edx_after, ARG_SNAP);
        }
    }

    for (int i = 0; i < MAX_ARGS; i++) {
        if (!s.arg_valid[i]) continue;
        uint8_t arg_after[ARG_SNAP];
        if (safe_copy(arg_after, (void *)s.args[i], ARG_SNAP)) {
            char label[32];
            snprintf(label, sizeof(label), "arg%d(0x%08X)", i, s.args[i]);
            diff_mem(label, s.arg_snapshots[i], arg_after, ARG_SNAP);
        }
    }
}

/* ---- installer ------------------------------------------------------------ */

static std::once_flag g_patches_installed;

/* Patch TerSafe's CreateObj vtable slots with our naked observation
 * trampolines. Called either from the LoadLibrary hook (first-time catch)
 * or from the fallback polling path. Runs at most once per process. */
static void install_tersafe_vtable_patches(HMODULE hts) {
    if (!hts) return;

    std::call_once(g_patches_installed, [hts]{
        typedef void *(__cdecl *fn_CO)(void);
        fn_CO pCO = (fn_CO)GetProcAddress(hts, "CreateObj");
        if (!pCO) { log("[TS] CreateObj export not found\n"); return; }

        g_tersafe_obj = pCO();
        if (!g_tersafe_obj) { log("[TS] CreateObj returned NULL\n"); return; }

        void **vtable = *(void ***)g_tersafe_obj;
        log("[TS] CreateObj -> %p  vtable=%p\n", g_tersafe_obj, vtable);
        for (int i = 0; i < NUM_SLOTS; i++) {
            g_real_vt[i] = vtable[i];
            log("[TS]   vt[%d:%s] = %p\n", i, VT_NAMES[i], vtable[i]);
        }

        /* ---- Sub-object probe ---------------------------------------------
         * For each embedded pointer in the TsCreateObj header, read its
         * first dword and classify: vtable-in-TerSafe-image (= C++ sub-object)
         * vs anything else (= data buffer / raw heap). */
        MODULEINFO mi = {};
        GetModuleInformation(GetCurrentProcess(), hts, &mi, sizeof mi);
        uintptr_t ts_lo = (uintptr_t)mi.lpBaseOfDll;
        uintptr_t ts_hi = ts_lo + mi.SizeOfImage;
        log("[TS] TerSafe image range: %p .. %p\n", (void*)ts_lo, (void*)ts_hi);

        static const int probe_offs[] = {0x0C, 0x10, 0x38, 0x70, 0x74, 0xEC, 0xFC};
        for (size_t i = 0; i < sizeof probe_offs / sizeof probe_offs[0]; i++) {
            int off = probe_offs[i];
            void *sub = *(void **)((uint8_t *)g_tersafe_obj + off);
            log("[TS]   +0x%02X: ptr=%p", off, sub);
            if (!sub) { log("  (NULL)\n"); continue; }
            if (IsBadReadPtr(sub, 4)) { log("  (UNREADABLE)\n"); continue; }
            void *first = *(void **)sub;
            uintptr_t fu = (uintptr_t)first;
            const char *kind =
                (fu >= ts_lo && fu < ts_hi)    ? "VTABLE (in TerSafe.dll .rdata)" :
                !first                         ? "zero (data/uninit)" :
                IsBadReadPtr(first, 4)         ? "bad-read (data)" :
                                                 "non-vtable heap (likely data)";
            log("  first_dword=%p  -> %s\n", first, kind);
            if (fu >= ts_lo && fu < ts_hi) {
                for (int k = 0; k < 4; k++) {
                    void *m = ((void **)first)[k];
                    log("[TS]       vt[%d] = %p\n", k, m);
                }
            }
        }
        /* ------------------------------------------------------------------ */

        DWORD old_vt_prot;
        if (!VirtualProtect(vtable, NUM_SLOTS * sizeof(void *), PAGE_READWRITE, &old_vt_prot)) {
            log("[TS] VirtualProtect(vtable) FAILED (err=%lu)\n", GetLastError());
            return;
        }

        for (int slot = 0; slot < NUM_SLOTS; slot++) {
            uint8_t *tramp = (uint8_t *)VirtualAlloc(
                nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (!tramp) {
                log("[TS] VirtualAlloc(tramp[%d]) FAILED (err=%lu)\n", slot, GetLastError());
                continue;
            }
            memset(tramp, 0xCC, 0x1000);

            uint8_t *p = tramp;
            auto w8  = [&](uint8_t  b) { *p++ = b; };
            auto w32 = [&](uint32_t v) { *(uint32_t *)p = v; p += 4; };
            auto emit_call_rel = [&](void *tgt) {
                w8(0xE8);
                int32_t rel = (int32_t)((intptr_t)tgt - (intptr_t)(p + 4));
                w32((uint32_t)rel);
            };

            SlotState &s = g_slot[slot];

            /* Save R_caller, this, edx. */
            w8(0x58);
            w8(0xA3); w32((uint32_t)&s.caller);
            w8(0x89); w8(0x0D); w32((uint32_t)&s.this_ptr);
            w8(0x89); w8(0x15); w32((uint32_t)&s.edx);

            /* Copy [esp+0..+28] to s.args[0..7]. */
            for (int i = 0; i < MAX_ARGS; i++) {
                int off = i * 4;
                if (off == 0) { w8(0x8B); w8(0x04); w8(0x24); }
                else          { w8(0x8B); w8(0x44); w8(0x24); w8((uint8_t)off); }
                w8(0xA3); w32((uint32_t)&s.args[i]);
            }

            /* observer_before(slot). */
            w8(0x60); w8(0x9C);
            w8(0x6A); w8((uint8_t)slot);
            emit_call_rel((void *)&tersafe_observer_before);
            w8(0x83); w8(0xC4); w8(0x04);
            w8(0x9D); w8(0x61);

            /* Measure esp, CALL real vt[slot], capture esp + retval. */
            w8(0x89); w8(0x25); w32((uint32_t)&s.esp_before);
            w8(0xFF); w8(0x15); w32((uint32_t)&g_real_vt[slot]);
            w8(0x89); w8(0x25); w32((uint32_t)&s.esp_after);
            w8(0xA3); w32((uint32_t)&s.retval);

            /* observer_after(slot). */
            w8(0x60); w8(0x9C);
            w8(0x6A); w8((uint8_t)slot);
            emit_call_rel((void *)&tersafe_observer_after);
            w8(0x83); w8(0xC4); w8(0x04);
            w8(0x9D); w8(0x61);

            /* Return to CryGame. */
            w8(0xFF); w8(0x35); w32((uint32_t)&s.caller);
            w8(0xC3);

            vtable[slot] = (void *)tramp;
            log("[TS] vt[%d:%s] -> tramp %p (size=%ld)  real=%p\n",
                slot, VT_NAMES[slot], tramp, (long)(p - tramp), g_real_vt[slot]);
        }

        VirtualProtect(vtable, NUM_SLOTS * sizeof(void *), old_vt_prot, &old_vt_prot);
        log("[TS] All %d vtable slots hooked with full-observation trampolines.\n", NUM_SLOTS);

        /* ---- Sub-vtable hooking (sub_obj_0c, sub_obj_10) -----------------
         * Hook up to SUB_SLOTS entries per sub-vtable.  Stop at the first
         * slot whose current target is NOT inside TerSafe's image — that's
         * the natural boundary between one class's vtable and the next. */
        static const int sub_offs[NUM_SUBS] = {0x0C, 0x10};
        for (int sub_idx = 0; sub_idx < NUM_SUBS; sub_idx++) {
            void *sub_obj = *(void **)((uint8_t *)g_tersafe_obj + sub_offs[sub_idx]);
            if (!sub_obj || IsBadReadPtr(sub_obj, 4)) {
                log("[TS] sub%d: missing/unreadable sub-object\n", sub_idx);
                continue;
            }
            void **sub_vt = *(void ***)sub_obj;
            if (!sub_vt || IsBadReadPtr(sub_vt, sizeof(void*))) {
                log("[TS] sub%d: missing/unreadable sub-vtable\n", sub_idx);
                continue;
            }
            log("[TS] sub%d: obj=%p vtable=%p\n", sub_idx, sub_obj, sub_vt);

            DWORD old_sub_prot;
            if (!VirtualProtect(sub_vt, SUB_SLOTS * sizeof(void *),
                                PAGE_READWRITE, &old_sub_prot)) {
                log("[TS] sub%d: VirtualProtect FAILED (err=%lu)\n",
                    sub_idx, GetLastError());
                continue;
            }

            int hooked = 0;
            for (int slot = 0; slot < SUB_SLOTS; slot++) {
                void *cur = sub_vt[slot];
                uintptr_t cu = (uintptr_t)cur;
                if (cu < ts_lo || cu >= ts_hi) {
                    log("[TS] sub%d_vt[%d] = %p  OUT OF RANGE — stopping\n",
                        sub_idx, slot, cur);
                    break;
                }
                g_real_sub[sub_idx][slot] = cur;

                uint8_t *tramp = (uint8_t *)VirtualAlloc(
                    nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
                if (!tramp) { log("[TS] sub%d_vt[%d] tramp alloc failed\n", sub_idx, slot); continue; }
                memset(tramp, 0xCC, 0x1000);

                uint8_t *p = tramp;
                auto w8  = [&](uint8_t  b) { *p++ = b; };
                auto w32 = [&](uint32_t v) { *(uint32_t *)p = v; p += 4; };
                auto emit_call_rel = [&](void *tgt) {
                    w8(0xE8);
                    int32_t rel = (int32_t)((intptr_t)tgt - (intptr_t)(p + 4));
                    w32((uint32_t)rel);
                };

                SlotState &s = g_sub_slot[sub_idx][slot];
                int encoded = (sub_idx << 8) | slot;

                /* pop ret-addr into caller; save this (ecx) and edx. */
                w8(0x58);
                w8(0xA3); w32((uint32_t)&s.caller);
                w8(0x89); w8(0x0D); w32((uint32_t)&s.this_ptr);
                w8(0x89); w8(0x15); w32((uint32_t)&s.edx);

                for (int i = 0; i < MAX_ARGS; i++) {
                    int off = i * 4;
                    if (off == 0) { w8(0x8B); w8(0x04); w8(0x24); }
                    else          { w8(0x8B); w8(0x44); w8(0x24); w8((uint8_t)off); }
                    w8(0xA3); w32((uint32_t)&s.args[i]);
                }

                /* observer_before(encoded). push is byte-sized so encoded
                 * must fit in 8 bits if we want `push imm8`; instead use
                 * `push imm32` (opcode 0x68) to support encoded=0..0x01FF. */
                w8(0x60); w8(0x9C);
                w8(0x68); w32((uint32_t)encoded);
                emit_call_rel((void *)&tersafe_sub_observer_before);
                w8(0x83); w8(0xC4); w8(0x04);
                w8(0x9D); w8(0x61);

                /* CALL real sub-vt slot. */
                w8(0x89); w8(0x25); w32((uint32_t)&s.esp_before);
                w8(0xFF); w8(0x15); w32((uint32_t)&g_real_sub[sub_idx][slot]);
                w8(0x89); w8(0x25); w32((uint32_t)&s.esp_after);
                w8(0xA3); w32((uint32_t)&s.retval);

                /* observer_after(encoded). */
                w8(0x60); w8(0x9C);
                w8(0x68); w32((uint32_t)encoded);
                emit_call_rel((void *)&tersafe_sub_observer_after);
                w8(0x83); w8(0xC4); w8(0x04);
                w8(0x9D); w8(0x61);

                /* Return via saved caller. */
                w8(0xFF); w8(0x35); w32((uint32_t)&s.caller);
                w8(0xC3);

                sub_vt[slot] = (void *)tramp;
                hooked++;
                log("[TS] sub%d_vt[%d] -> tramp %p  real=%p\n",
                    sub_idx, slot, tramp, cur);
            }
            VirtualProtect(sub_vt, SUB_SLOTS * sizeof(void *), old_sub_prot, &old_sub_prot);
            log("[TS] sub%d: %d slot(s) hooked\n", sub_idx, hooked);
        }
        /* ------------------------------------------------------------------ */
    });
}

/* ---- LoadLibrary hook -----------------------------------------------------
 *
 * TerSafe.dll is loaded ~26 s into game startup via CryGame's LoadLibrary
 * call. A polling thread can't reliably install vtable patches before
 * CryGame calls CreateObj() + vt_0(). Instead, we inline-hook kernel32's
 * LoadLibraryA/W/ExA/ExW: when any of them returns with an HMODULE, we
 * check the filename and, if it's TerSafe, install patches SYNCHRONOUSLY
 * before the caller sees the HMODULE. That guarantees vt_0 is caught.
 *
 * Mechanism: 5-byte `jmp rel32` inline patch at the target's entry + a
 * per-function trampoline that re-executes the original 5 bytes and jumps
 * to target+5 so we can still invoke the real function.
 * --------------------------------------------------------------------------- */

typedef HMODULE (WINAPI *LoadLibA_t)   (LPCSTR);
typedef HMODULE (WINAPI *LoadLibW_t)   (LPCWSTR);
typedef HMODULE (WINAPI *LoadLibExA_t) (LPCSTR,  HANDLE, DWORD);
typedef HMODULE (WINAPI *LoadLibExW_t) (LPCWSTR, HANDLE, DWORD);

static LoadLibA_t    real_LoadLibraryA_via_tramp   = nullptr;
static LoadLibW_t    real_LoadLibraryW_via_tramp   = nullptr;
static LoadLibExA_t  real_LoadLibraryExA_via_tramp = nullptr;
static LoadLibExW_t  real_LoadLibraryExW_via_tramp = nullptr;

static bool is_tersafe_module(HMODULE h) {
    if (!h) return false;
    char path[MAX_PATH];
    if (!GetModuleFileNameA(h, path, MAX_PATH)) return false;
    const char *name = strrchr(path, '\\');
    name = name ? name + 1 : path;
    return _stricmp(name, "tersafe.dll") == 0;
}

static void on_module_loaded(HMODULE h, const char *via) {
    if (!is_tersafe_module(h)) return;
    log("[TS] LoadLibrary hook caught TerSafe via %s -> HMODULE=%p\n", via, h);
    install_tersafe_vtable_patches(h);
}

static HMODULE WINAPI my_LoadLibraryA(LPCSTR p) {
    HMODULE h = real_LoadLibraryA_via_tramp(p);
    on_module_loaded(h, "LoadLibraryA");
    return h;
}
static HMODULE WINAPI my_LoadLibraryW(LPCWSTR p) {
    HMODULE h = real_LoadLibraryW_via_tramp(p);
    on_module_loaded(h, "LoadLibraryW");
    return h;
}
static HMODULE WINAPI my_LoadLibraryExA(LPCSTR p, HANDLE f, DWORD flags) {
    HMODULE h = real_LoadLibraryExA_via_tramp(p, f, flags);
    on_module_loaded(h, "LoadLibraryExA");
    return h;
}
static HMODULE WINAPI my_LoadLibraryExW(LPCWSTR p, HANDLE f, DWORD flags) {
    HMODULE h = real_LoadLibraryExW_via_tramp(p, f, flags);
    on_module_loaded(h, "LoadLibraryExW");
    return h;
}

/* Sanity check: the first 5 bytes of a function must not contain any
 * relative-addressed instructions (E8 call, E9/EB jmp, 70-7F jcc short).
 * We're copying them verbatim into the trampoline, so any pc-relative
 * fixup would be wrong. */
static bool safe_to_hotpatch(const uint8_t *bytes) {
    for (int i = 0; i < 5; i++) {
        uint8_t b = bytes[i];
        if (b == 0xE8 || b == 0xE9 || b == 0xEB) return false;
        if (b >= 0x70 && b <= 0x7F) return false;     /* short jcc */
        if (b == 0xC3 || b == 0xC2) return false;     /* near ret */
    }
    return true;
}

/* Install a 5-byte jmp at fn's entry, return a trampoline that runs the
 * original 5 bytes + jmps back to fn+5. */
static void *inline_patch(void *fn, void *hook, const char *name) {
    uint8_t *target = (uint8_t *)fn;
    if (!safe_to_hotpatch(target)) {
        log("[TS] inline_patch(%s): first 5 bytes not relocatable (%02X %02X %02X %02X %02X)\n",
            name, target[0], target[1], target[2], target[3], target[4]);
        return nullptr;
    }

    uint8_t *tramp = (uint8_t *)VirtualAlloc(
        nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!tramp) {
        log("[TS] inline_patch(%s): VirtualAlloc FAILED (err=%lu)\n", name, GetLastError());
        return nullptr;
    }
    memset(tramp, 0xCC, 0x1000);

    /* Trampoline: [5 bytes of target] + [E9 rel32 back to target+5] */
    memcpy(tramp, target, 5);
    tramp[5] = 0xE9;
    int32_t rel_back = (int32_t)((intptr_t)(target + 5) - (intptr_t)(tramp + 5 + 5));
    *(int32_t *)(tramp + 6) = rel_back;

    /* Overwrite target with `jmp rel32` to our hook. */
    DWORD old_prot;
    if (!VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &old_prot)) {
        log("[TS] inline_patch(%s): VirtualProtect FAILED (err=%lu)\n", name, GetLastError());
        VirtualFree(tramp, 0, MEM_RELEASE);
        return nullptr;
    }
    target[0] = 0xE9;
    int32_t rel_fwd = (int32_t)((intptr_t)hook - (intptr_t)(target + 5));
    *(int32_t *)(target + 1) = rel_fwd;
    VirtualProtect(target, 5, old_prot, &old_prot);

    log("[TS] inline_patch(%s): target=%p hook=%p tramp=%p\n", name, target, hook, tramp);
    return tramp;
}

static void install_loadlibrary_hooks() {
    HMODULE k32 = GetModuleHandleA("kernel32.dll");
    if (!k32) { log("[TS] kernel32 not found\n"); return; }

    void *la  = (void *)GetProcAddress(k32, "LoadLibraryA");
    void *lw  = (void *)GetProcAddress(k32, "LoadLibraryW");
    void *lae = (void *)GetProcAddress(k32, "LoadLibraryExA");
    void *lwe = (void *)GetProcAddress(k32, "LoadLibraryExW");

    if (la)  real_LoadLibraryA_via_tramp   = (LoadLibA_t)   inline_patch(la,  (void *)my_LoadLibraryA,   "LoadLibraryA");
    if (lw)  real_LoadLibraryW_via_tramp   = (LoadLibW_t)   inline_patch(lw,  (void *)my_LoadLibraryW,   "LoadLibraryW");
    if (lae) real_LoadLibraryExA_via_tramp = (LoadLibExA_t) inline_patch(lae, (void *)my_LoadLibraryExA, "LoadLibraryExA");
    if (lwe) real_LoadLibraryExW_via_tramp = (LoadLibExW_t) inline_patch(lwe, (void *)my_LoadLibraryExW, "LoadLibraryExW");
}

/* ---- public entry --------------------------------------------------------- */

void install_tersafe_hooks() {
    log("[TS] install_tersafe_hooks: DISABLED\n");
    return;

    install_loadlibrary_hooks();
    log("[TS] LoadLibrary hooks armed — waiting for TerSafe.dll to load.\n");

    HMODULE hts = GetModuleHandleA("tersafe.dll");
    if (hts) {
        log("[TS] TerSafe already loaded at %p — installing patches immediately.\n", hts);
        install_tersafe_vtable_patches(hts);
    }
}
