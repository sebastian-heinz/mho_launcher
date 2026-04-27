#include "hooks_mhoclient.h"
#include "hooks_mho_handlers.h"
#include "log.h"
#include "hook_table.h"
#include "ag_ini.h"
#include "win_util.h"

#include <string>

DWORD server_url_address = 0;

static bool g_log_client_logs = false;

static const int IGNORE_LOGS_SIZE = 9;
static std::string IGNORE_LOGS[IGNORE_LOGS_SIZE] = {
    "$4[Error] I3DEngine::LoadStatObj: filename is not specified",
    "$6[Warning] Texture does not exist",
    "$6[Warning] Warning: CTexMan::ImagePreprocessing",
    "$6[Warning] Error: CTexMan::ImagePreprocessing:",
    "$6[Warning] CAF-File Not Found:",
    "$6[Warning] Normal map should have '_ddn' suffix in filename",
    "$6[Warning] CryAnimation: process Aimpose",
    "$6[Warning] Reference if not existing Joint-Name. Aim-IK disabled",
    "$6[Warning] Failed to create animation alias"
};

static void client_log(int do_log, char *near_log_ptr) {
    if (!g_log_client_logs) {
        return;
    }
    if (do_log == 0) {
        return;
    }
    if (near_log_ptr == nullptr) {
        return;
    }
    char *log_ptr = near_log_ptr + 0x20;
    int log_len = 0;
    while (log_ptr[log_len] != 0) {
        log_len++;
    }
    if (log_len <= 0) {
        return;
    }
    std::string log_text = std::string(log_ptr, log_len);

    for (int i = 0; i < IGNORE_LOGS_SIZE; i++) {
        if (log_text.find(IGNORE_LOGS[i]) != std::string::npos) {
            return;
        }
    }

    log("client_log: %s \n", log_text.c_str());
}

// --- hook callback ---

static void on_client_log(HookContext *ctx) {
    if (!ctx) return;
    client_log((int)ctx->ecx, (char *)ctx);
}

// Stolen bytes at mhoclient + 0x3E0F06:
//   mov esp, ebp   (8B E5)
//   pop ebp        (5D)
//   ret 0xC        (C2 0C 00)
// = 6 bytes. The ret handles the return, jmp-back is dead code.
static const uint8_t client_log_stolen[] = {0x8B, 0xE5, 0x5D, 0xC2, 0x0C, 0x00};

static const JmpHookEntry mhoclient_jmp_hooks[] = {
    {0x3E0F06, on_client_log, client_log_stolen, sizeof(client_log_stolen)},
};

static const NopPatchEntry mhoclient_nops[] = {
    {0x3E0C82, 6},
};

// --- install ---

void install_mhoclient_hooks(DWORD mhoclient_base) {
    server_url_address = mhoclient_base + 0x157AAA0;
    log("server_url_address: 0x%08X \n", server_url_address);
    log("mhoclient_base: 0x%08X \n", mhoclient_base);

    std::wstring ini_path = get_exe_dir() + L"ag_mho.ini";
    auto ag_cfg = ag_ini_read(ini_path);
    g_log_client_logs = ag_ini_get_int(ag_cfg, "log_client_logs", 0) != 0;
    log("config log_client_logs = %d \n", g_log_client_logs ? 1 : 0);

    install_jmp_hooks(mhoclient_base, mhoclient_jmp_hooks, std::size(mhoclient_jmp_hooks));
    install_nop_patches(mhoclient_base, mhoclient_nops, std::size(mhoclient_nops));
    log("mhoclient hooks: client_log ENABLED\n");

    install_mho_handler_hooks(mhoclient_base);
}
