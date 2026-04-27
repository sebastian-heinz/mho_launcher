#include "hooks_ods.h"
#include "log.h"
#include "patch.h"

#include <windows.h>
#include <cstring>

// 5-byte rel32 jmp detour over the prologue of OutputDebugStringA/W.
// We don't call the original — we swallow the call and funnel the string
// through our own log pipeline. That's fine because the only reason a
// consumer would need the original behavior is an attached debugger, and
// we're trying to replace that role, not coexist with it.

static void __stdcall ods_a_detour(const char *s) {
    if (!s) return;
    size_t n = strnlen(s, 8192);
    bool has_nl = n > 0 && s[n - 1] == '\n';
    log("[ODS] %.*s%s", (int)n, s, has_nl ? "" : "\n");
}

static void __stdcall ods_w_detour(const wchar_t *s) {
    if (!s) return;
    char buf[4096];
    int n = WideCharToMultiByte(CP_UTF8, 0, s, -1, buf, sizeof(buf), NULL, NULL);
    if (n <= 0) return;
    // n includes the terminating NUL
    size_t len = (size_t)n - 1;
    bool has_nl = len > 0 && buf[len - 1] == '\n';
    log("[ODS] %.*s%s", (int)len, buf, has_nl ? "" : "\n");
}

void install_ods_hooks() {
    HMODULE h = GetModuleHandleA("kernel32.dll");
    if (!h) {
        log("install_ods_hooks: kernel32.dll not found\n");
        return;
    }
    FARPROC a = GetProcAddress(h, "OutputDebugStringA");
    FARPROC w = GetProcAddress(h, "OutputDebugStringW");
    if (a) {
        patch_jmp((DWORD)0, (DWORD)a, (void *)ods_a_detour);
        log("install_ods_hooks: OutputDebugStringA hooked at %p\n", (void *)a);
    } else {
        log("install_ods_hooks: OutputDebugStringA not found\n");
    }
    if (w) {
        patch_jmp((DWORD)0, (DWORD)w, (void *)ods_w_detour);
        log("install_ods_hooks: OutputDebugStringW hooked at %p\n", (void *)w);
    } else {
        log("install_ods_hooks: OutputDebugStringW not found\n");
    }
}
