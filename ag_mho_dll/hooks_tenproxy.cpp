#include "hooks_tenproxy.h"
#include "log.h"
#include "hook_table.h"
#include "str_util.h"

#include <string>

// --- callback ---

static void tenproxy_log(wchar_t *log_ptr) {
    std::wstring w_log_txt = std::wstring(log_ptr);
    std::string log_text = ws_2_s(w_log_txt);
    log("tenproxy_log: %s \n", log_text.c_str());
}

// --- hook context callback ---

static void on_tenproxy_log(HookContext *ctx) {
    // Original asm passed eax as the log pointer
    tenproxy_log((wchar_t *)ctx->eax);
}

// Stolen bytes at tenproxy + 0xA143:
//   pop edi       (5F)
//   pop esi       (5E)
//   pop ebx       (5B)
//   mov esp,ebp   (8B E5)
//   pop ebp       (5D)
//   ret 0x14      (C2 14 00)
// = 9 bytes. The ret handles the return, jmp-back is dead code.
static const uint8_t tenproxy_log_stolen[] = {0x5F, 0x5E, 0x5B, 0x8B, 0xE5, 0x5D, 0xC2, 0x14, 0x00};

static const JmpHookEntry tenproxy_jmp_hooks[] = {
    {0xA143, on_tenproxy_log, tenproxy_log_stolen, sizeof(tenproxy_log_stolen)},
};

static const NopPatchEntry tenproxy_nops[] = {
    {0xA0D3, 8},
};

// --- install ---

void install_tenproxy_hooks() {
    DWORD base = wait_for_module("tenproxy.dll");
    log("got tenproxy: %p \n", (void *)base);

    install_jmp_hooks(base, tenproxy_jmp_hooks, std::size(tenproxy_jmp_hooks));
    install_nop_patches(base, tenproxy_nops, std::size(tenproxy_nops));
}
