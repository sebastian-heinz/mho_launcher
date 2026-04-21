#include "hooks_tersafe.h"
#include "log.h"
#include "hook_table.h"
#include "str_util.h"
#include "win_util.h"

#include <windows.h>
#include <cstdint>
#include <cstring>
#include <string>

/*
 * TerSafe vtable interception.
 * Swaps CreateObj's vtable entries with proxy functions that:
 *   1. Log all arguments
 *   2. Call the REAL function
 *   3. Log the return value
 *
 * This lets us observe the real TerSafe's behavior without modifying it.
 */

// Original vtable function pointers (filled at runtime)
static void *g_real_vt[6] = {};
static void *g_tersafe_obj = nullptr;

// Helper: dump N bytes as hex, only show non-zero dwords
static void dump_mem(const char *label, const void *ptr, int size) {
    if (!ptr) return;
    const uint32_t *d = (const uint32_t *)ptr;
    int dwords = size / 4;
    for (int i = 0; i < dwords; i++) {
        if (d[i] != 0) {
            log("  %s[+0x%02X] = 0x%08X\n", label, i*4, d[i]);
        }
    }
}

// Helper: compare before/after and log differences
static void diff_mem(const char *label, const uint8_t *before, const uint8_t *after, int size) {
    const uint32_t *b = (const uint32_t *)before;
    const uint32_t *a = (const uint32_t *)after;
    int dwords = size / 4;
    int changes = 0;
    for (int i = 0; i < dwords; i++) {
        if (b[i] != a[i]) {
            log("  %s[+0x%02X] CHANGED: 0x%08X → 0x%08X\n", label, i*4, b[i], a[i]);
            changes++;
        }
    }
    if (changes == 0) {
        log("  %s: no changes\n", label);
    }
}

#define SNAP_SIZE 0x500  // capture full object (1280 bytes)
#define ARG_SNAP  0x40   // capture 64 bytes of arg buffer

// Safe memcpy that won't crash on bad pointers
static bool safe_copy(void *dst, const void *src, size_t n) {
    if (!src || IsBadReadPtr(src, n)) return false;
    memcpy(dst, src, n);
    return true;
}

static int __fastcall proxy_vt0(void *ecx, void *edx, void *initStruct) {
    uint8_t obj_before[SNAP_SIZE] = {};
    uint8_t arg_before[ARG_SNAP] = {};
    uint8_t obj_after[SNAP_SIZE] = {};
    uint8_t arg_after[ARG_SNAP] = {};

    log("[TS:vt0] this=%p arg=%p\n", ecx, initStruct);
    if (initStruct) {
        safe_copy(arg_before, initStruct, ARG_SNAP);
        log("  arg BEFORE:\n");
        dump_mem("arg", initStruct, 32);
    }
    safe_copy(obj_before, ecx, SNAP_SIZE);

    typedef int (__fastcall *Fn)(void*, void*, void*);
    int ret = ((Fn)g_real_vt[0])(ecx, edx, initStruct);

    safe_copy(obj_after, ecx, SNAP_SIZE);
    log("[TS:vt0] → returned %d\n", ret);
    diff_mem("obj", obj_before, obj_after, SNAP_SIZE);
    if (initStruct) {
        safe_copy(arg_after, initStruct, ARG_SNAP);
        diff_mem("arg", arg_before, arg_after, ARG_SNAP);
    }
    return ret;
}

static int __fastcall proxy_vt1(void *ecx, void *edx, void *dataBuf) {
    uint8_t obj_before[SNAP_SIZE] = {};
    uint8_t arg_before[ARG_SNAP] = {};
    uint8_t obj_after[SNAP_SIZE] = {};
    uint8_t arg_after[ARG_SNAP] = {};

    log("[TS:vt1] this=%p buf=%p\n", ecx, dataBuf);
    safe_copy(obj_before, ecx, SNAP_SIZE);
    if (dataBuf) safe_copy(arg_before, dataBuf, ARG_SNAP);

    typedef int (__fastcall *Fn)(void*, void*, void*);
    int ret = ((Fn)g_real_vt[1])(ecx, edx, dataBuf);

    safe_copy(obj_after, ecx, SNAP_SIZE);
    log("[TS:vt1] → returned %d\n", ret);
    diff_mem("obj", obj_before, obj_after, SNAP_SIZE);
    if (dataBuf) {
        safe_copy(arg_after, dataBuf, ARG_SNAP);
        diff_mem("arg", arg_before, arg_after, ARG_SNAP);
    }
    /* Full hex dump of object after vt1 (handshake state) */
    log("[TS:vt1] FULL OBJECT DUMP (0x%X bytes):\n", SNAP_SIZE);
    for (int i = 0; i < SNAP_SIZE; i += 16) {
        uint8_t *p = obj_after + i;
        bool allzero = true;
        for (int j = 0; j < 16; j++) if (p[j]) { allzero = false; break; }
        if (!allzero) {
            log("  +%03X: %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n",
                i, p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],
                p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15]);
        }
    }
    return ret;
}

static int __fastcall proxy_vt2(void *ecx, void *edx, void *pktWrapper) {
    uint8_t obj_before[SNAP_SIZE] = {};
    uint8_t arg_before[ARG_SNAP] = {};
    uint8_t obj_after[SNAP_SIZE] = {};
    uint8_t arg_after[ARG_SNAP] = {};
    uint8_t data_before[256] = {};
    uint8_t data_after[256] = {};

    uint32_t cmd = 0, data_len = 0;
    void *data_ptr = NULL;
    if (pktWrapper) {
        cmd = ((uint32_t*)pktWrapper)[0];
        data_len = ((uint32_t*)pktWrapper)[2];
        data_ptr = (void*)((uint32_t*)pktWrapper)[7];  /* offset 0x1C = wrapper[7] */
    }

    log("[TS:vt2] this=%p pkt=%p cmd=0x%X len=%d data=%p\n", ecx, pktWrapper, cmd, data_len, data_ptr);
    safe_copy(obj_before, ecx, SNAP_SIZE);
    if (pktWrapper) safe_copy(arg_before, pktWrapper, ARG_SNAP);
    /* Capture actual packet data BEFORE */
    int snap_data_len = (data_len > 0 && data_len < 256) ? data_len : 64;
    if (data_ptr) safe_copy(data_before, data_ptr, snap_data_len);

    typedef int (__fastcall *Fn)(void*, void*, void*);
    int ret = ((Fn)g_real_vt[2])(ecx, edx, pktWrapper);

    safe_copy(obj_after, ecx, SNAP_SIZE);
    log("[TS:vt2] cmd=0x%X → returned %d\n", cmd, ret);
    diff_mem("obj", obj_before, obj_after, SNAP_SIZE);
    if (pktWrapper) {
        safe_copy(arg_after, pktWrapper, ARG_SNAP);
        diff_mem("wrapper", arg_before, arg_after, ARG_SNAP);
    }
    /* Diff actual packet data AFTER */
    if (data_ptr) {
        safe_copy(data_after, data_ptr, snap_data_len);
        diff_mem("pkt_data", data_before, data_after, snap_data_len);
    }
    /* Also check if the data_ptr itself changed */
    if (pktWrapper) {
        void *data_ptr_after = (void*)((uint32_t*)pktWrapper)[7];
        if (data_ptr_after != data_ptr) {
            log("  data_ptr CHANGED: %p → %p\n", data_ptr, data_ptr_after);
        }
    }
    return ret;
}

static int __fastcall proxy_vt3(void *ecx, void *edx, uint32_t param) {
    uint8_t obj_before[SNAP_SIZE] = {};
    uint8_t obj_after[SNAP_SIZE] = {};

    log("[TS:vt3] this=%p param=0x%X\n", ecx, param);
    safe_copy(obj_before, ecx, SNAP_SIZE);

    typedef int (__fastcall *Fn)(void*, void*, uint32_t);
    int ret = ((Fn)g_real_vt[3])(ecx, edx, param);

    safe_copy(obj_after, ecx, SNAP_SIZE);
    log("[TS:vt3] param=0x%X → returned %d\n", param, ret);
    diff_mem("obj", obj_before, obj_after, SNAP_SIZE);
    return ret;
}

static int __fastcall proxy_vt4(void *ecx, void *edx) {
    log("[TS:vt4] this=%p\n", ecx);
    typedef int (__fastcall *Fn)(void*, void*);
    int ret = ((Fn)g_real_vt[4])(ecx, edx);
    log("[TS:vt4] → returned %d\n", ret);
    return ret;
}

static int __fastcall proxy_vt5(void *ecx, void *edx) {
    log("[TS:vt5] this=%p\n", ecx);
    typedef int (__fastcall *Fn)(void*, void*);
    int ret = ((Fn)g_real_vt[5])(ecx, edx);
    log("[TS:vt5] → returned %d\n", ret);
    return ret;
}

static void *g_proxy_vtable[8] = {
    (void*)proxy_vt0, (void*)proxy_vt1, (void*)proxy_vt2,
    (void*)proxy_vt3, (void*)proxy_vt4, (void*)proxy_vt5,
    nullptr, nullptr
};

void install_tersafe_hooks() {
    return;

    /* Wait for TerSafe.dll to load */
    HMODULE hts = nullptr;
    for (int i = 0; i < 600 && !hts; i++) {
        hts = GetModuleHandleA("tersafe.dll");
        if (!hts) hts = GetModuleHandleA("tersafe");
        if (!hts) std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (!hts) { log("[TS] TerSafe not found\n"); return; }
    log("[TS] TerSafe at %p\n", hts);

    /* No wait — catch vt0 Init immediately */

    /* Call CreateObj to get the object */
    typedef void *(__cdecl *fn_CO)(void);
    fn_CO pCO = (fn_CO)GetProcAddress(hts, "CreateObj");
    if (!pCO) { log("[TS] CreateObj not found\n"); return; }

    g_tersafe_obj = pCO();
    log("[TS] CreateObj → %p\n", g_tersafe_obj);
    if (!g_tersafe_obj) return;

    /* Read original vtable */
    void **vtable = *(void ***)g_tersafe_obj;
    log("[TS] vtable at %p\n", vtable);
    for (int i = 0; i < 6; i++) {
        g_real_vt[i] = vtable[i];
        log("[TS]   vt[%d] = %p\n", i, vtable[i]);
    }

    /* Patch individual vtable entries IN-PLACE (don't change vtable pointer).
     * This avoids breaking code that checks the vtable pointer value. */
    DWORD old_protect;
    if (VirtualProtect(vtable, 24, PAGE_READWRITE, &old_protect)) {
        vtable[0] = (void*)proxy_vt0;
        vtable[1] = (void*)proxy_vt1;
        vtable[2] = (void*)proxy_vt2;
        vtable[3] = (void*)proxy_vt3;
        vtable[4] = (void*)proxy_vt4;
        vtable[5] = (void*)proxy_vt5;
        VirtualProtect(vtable, 24, old_protect, &old_protect);
        log("[TS] Vtable entries patched IN-PLACE (ptr unchanged at %p)\n", vtable);
    } else {
        log("[TS] VirtualProtect FAILED on vtable at %p\n", vtable);
    }
    log("[TS] Ready — all calls will be logged with real return values\n");
}
