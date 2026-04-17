#include "dll_export.h"
#include "log.h"
#include "hooks_mhoclient.h"
#include "hooks_crygame.h"
#include "hooks_protocalhandler.h"
#include "hooks_tenproxy.h"

#ifdef __MINGW32__
#include "compat.h"
#endif

#include "str_util.h"
#include "win_util.h"
#include "shared_memory.h"

#include <windows.h>
#include <thread>
#include <string>
#include <iostream>
#include <tchar.h>

MHO_API_EXPORT void dummy() {}

static void CreateConsole() {
    if (!AllocConsole()) {
        return;
    }

    FILE *fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();

    HANDLE hConOut = CreateFile(_T("CONOUT$"),
                                GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hConIn = CreateFile(_T("CONIN$"),
                               GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
    SetStdHandle(STD_ERROR_HANDLE, hConOut);
    SetStdHandle(STD_INPUT_HANDLE, hConIn);
    std::wcout.clear();
    std::wclog.clear();
    std::wcerr.clear();
    std::wcin.clear();

    std::ios::sync_with_stdio();
}

static void run() {
    log_init();
    CreateConsole();
    log("run\n");

    TenProxyTclsSharedMeMemory tptsmm;
    tptsmm.map(0);

    std::wstring exe_name_w = get_exe_name();
    std::string exe_name = ws_2_s(exe_name_w);
    log("exe_name: %s \n", exe_name.c_str());

    HMODULE mho_client_handle = GetModuleHandleW(exe_name_w.c_str());
    DWORD mho_client_addr = (DWORD) mho_client_handle;
    log("mho_client_handle: %p \n", mho_client_handle);

    install_mhoclient_hooks(mho_client_addr);

    std::thread(install_crygame_hooks).detach();
    std::thread(install_protocalhandler_hooks).detach();
    std::thread(install_tenproxy_hooks).detach();
}

BOOL WINAPI DllMain(HINSTANCE h_instance, DWORD fdw_reason, LPVOID lpv_reserved) {
    switch (fdw_reason) {
        case DLL_PROCESS_ATTACH:
            std::thread(run).detach();
            break;
        case DLL_PROCESS_DETACH:
            log_shutdown();
            break;
        default:
            break;
    }
    return TRUE;
}
