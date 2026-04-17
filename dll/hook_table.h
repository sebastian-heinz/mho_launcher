#ifndef MHO_HOOK_TABLE_H
#define MHO_HOOK_TABLE_H

#include "patch.h"
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <thread>
#include <chrono>

// --- CPU register state at a jmp hook point ---
// Layout matches x86 pushad order (EDI pushed last = lowest address)
struct HookContext {
    DWORD edi, esi, ebp, _esp, ebx, edx, ecx, eax;
};

// Helper: pointer to the original stack at the hook point
// (the stack contents that were there before the trampoline ran)
inline DWORD *hook_stack(HookContext *ctx) {
    return (DWORD *)((char *)ctx + sizeof(HookContext));
}

typedef void (*HookCallback)(HookContext *ctx);

// --- Hook entry types ---

struct CallHookEntry {
    DWORD offset;
    void *target_fn;
};

// Declarative jmp hook: provide the offset, a callback, and the
// original instruction bytes at the hook site ("stolen bytes").
// The system generates a trampoline at runtime — no hand-written asm needed.
struct JmpHookEntry {
    DWORD offset;
    HookCallback callback;
    const uint8_t *stolen_bytes;
    int stolen_count;       // must be >= 5, aligned to instruction boundaries
};

struct NopPatchEntry {
    DWORD offset;
    int count;
};

// --- Runtime trampoline generator ---
// Allocates executable memory and writes x86 machine code:
//   pushad
//   lea eax, [esp]          ; HookContext* = top of saved regs
//   push eax                ; pass as argument
//   mov eax, <callback>
//   call eax
//   add esp, 4
//   popad
//   <stolen bytes>          ; original instructions, copied verbatim
//   jmp <return_addr>       ; back to original code after the hook site
//
// If the stolen bytes end with a ret instruction, the jmp is dead code.

inline void *generate_trampoline(DWORD return_addr, const uint8_t *stolen, int stolen_count, HookCallback callback) {
    int total = 21 + stolen_count;
    uint8_t *code = (uint8_t *)VirtualAlloc(NULL, total, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!code) return nullptr;

    int p = 0;
    code[p++] = 0x60;                                          // pushad
    code[p++] = 0x8D; code[p++] = 0x04; code[p++] = 0x24;    // lea eax, [esp]
    code[p++] = 0x50;                                          // push eax
    code[p++] = 0xB8;                                          // mov eax, imm32
    *(DWORD *)(code + p) = (DWORD)callback; p += 4;
    code[p++] = 0xFF; code[p++] = 0xD0;                       // call eax
    code[p++] = 0x83; code[p++] = 0xC4; code[p++] = 0x04;    // add esp, 4
    code[p++] = 0x61;                                          // popad
    memcpy(code + p, stolen, stolen_count); p += stolen_count; // stolen bytes
    code[p++] = 0xE9;                                          // jmp rel32
    *(DWORD *)(code + p) = return_addr - ((DWORD)(code + p) + 4);

    return code;
}

// --- Batch installers ---

inline void install_call_hooks(DWORD base, const CallHookEntry *entries, size_t count) {
    for (size_t i = 0; i < count; i++) {
        hook_call(base, entries[i].offset, entries[i].target_fn);
    }
}

inline void install_jmp_hooks(DWORD base, const JmpHookEntry *entries, size_t count) {
    for (size_t i = 0; i < count; i++) {
        DWORD hook_addr = base + entries[i].offset;
        DWORD return_addr = hook_addr + entries[i].stolen_count;
        void *trampoline = generate_trampoline(return_addr, entries[i].stolen_bytes,
                                                entries[i].stolen_count, entries[i].callback);
        // Log trampoline details for debugging
        {
            char buf[256];
            snprintf(buf, sizeof(buf),
                "jmp_hook: site=0x%08X stolen=%d trampoline=%p callback=%p ret=0x%08X\n",
                (unsigned)hook_addr, entries[i].stolen_count,
                trampoline, (void*)entries[i].callback, (unsigned)return_addr);
            OutputDebugStringA(buf);
        }
        patch_jmp(base, entries[i].offset, trampoline);
        // NOP remaining bytes beyond the 5-byte jmp for clean disassembly
        if (entries[i].stolen_count > 5) {
            for (int j = 5; j < entries[i].stolen_count; j++) {
                uint8_t nop = 0x90;
                WriteMemory((LPVOID)(hook_addr + j), &nop, 1);
            }
        }
    }
}

inline void install_nop_patches(DWORD base, const NopPatchEntry *entries, size_t count) {
    for (size_t i = 0; i < count; i++) {
        patch_nop(base, entries[i].offset, entries[i].count);
    }
}

// --- Module wait helper ---

inline DWORD wait_for_module(const char *module_name) {
    HMODULE handle = nullptr;
    while (!handle) {
        handle = GetModuleHandleA(module_name);
        if (!handle) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
    return (DWORD)handle;
}

#endif
