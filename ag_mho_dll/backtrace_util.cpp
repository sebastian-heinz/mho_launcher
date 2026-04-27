#include "backtrace_util.h"
#include "log.h"
#include "hook_table.h"  // HookContext, hook_stack

#include <windows.h>
#include <cstdint>
#include <cstdio>

struct ModuleEntry {
    const char *name;
    const char *display;
    DWORD       base;
    DWORD       span;
};

static ModuleEntry g_modules[] = {
    {"MHOClient.exe",       "mho",   0, 0x02000000},
    {"CryGame.dll",         "cry",   0, 0x02400000},
    {"CryAction.dll",       "cact",  0, 0x01900000},
    {"CrySystem.dll",       "csys",  0, 0x00800000},
    {"tenproxy.dll",        "tp",    0, 0x00400000},
    {"protocalhandler.dll", "ph",    0, 0x00400000},
    {"msvcr120.dll",        "msvcr", 0, 0x00400000},
    {"ntdll.dll",           "ntdll", 0, 0x00400000},
    {"kernel32.dll",        "k32",   0, 0x00400000},
};
static const size_t MODULE_COUNT = sizeof(g_modules) / sizeof(g_modules[0]);

static void refresh_module_bases() {
    for (size_t i = 0; i < MODULE_COUNT; i++) {
        if (g_modules[i].base == 0) {
            g_modules[i].base = (DWORD)GetModuleHandleA(g_modules[i].name);
        }
    }
}

static void format_addr(uint32_t addr, char *buf, size_t buf_sz) {
    for (size_t i = 0; i < MODULE_COUNT; i++) {
        ModuleEntry &m = g_modules[i];
        if ((m.base != 0) && (addr >= m.base) && (addr < m.base + m.span)) {
            snprintf(buf, buf_sz, "%s+0x%X", m.display, addr - m.base);
            return;
        }
    }
    snprintf(buf, buf_sz, "?+0x%X", addr);
}

static bool safe_read_dword(uint32_t addr, uint32_t *out) {
    if (addr < 0x10000) return false;
    SIZE_T n = 0;
    return ReadProcessMemory(
               GetCurrentProcess(),
               reinterpret_cast<LPCVOID>(static_cast<uintptr_t>(addr)),
               out, sizeof(uint32_t), &n) != 0
           && n == sizeof(uint32_t);
}

void log_caller_stack(HookContext *ctx, const char *tag, int max_depth) {
    if (ctx == nullptr || max_depth <= 0) return;
    if (max_depth > 32) max_depth = 32;
    refresh_module_bases();
    char fmt[64];

    // f0 = the function that called the hooked function. Trampoline pushed
    // saved_regs and the original return address; hook_stack returns the
    // "stack as the hooked function would see it", so stack[0] is the ret.
    DWORD *stk = hook_stack(ctx);
    uint32_t ret0 = (uint32_t)stk[0];
    format_addr(ret0, fmt, sizeof(fmt));
    log("[%s.bt] f0 ret=0x%08X %s\n", tag, ret0, fmt);

    // Walk EBP chain — works whenever each caller preserves the frame pointer.
    uint32_t ebp = (uint32_t)ctx->ebp;
    for (int i = 1; i <= max_depth; i++) {
        if (ebp < 0x10000 || ebp > 0xC0000000) break;
        uint32_t saved_ebp = 0, saved_ret = 0;
        if (!safe_read_dword(ebp, &saved_ebp))     break;
        if (!safe_read_dword(ebp + 4, &saved_ret)) break;
        if (saved_ret < 0x10000) break;
        format_addr(saved_ret, fmt, sizeof(fmt));
        log("[%s.bt] f%d ret=0x%08X %s\n", tag, i, saved_ret, fmt);
        if (saved_ebp <= ebp || saved_ebp > 0xC0000000) break;
        ebp = saved_ebp;
    }
}
