#include "win_util.h"
#include "str_util.h"
#include "shared_memory.h"
#include "ag_ini.h"

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

static std::ofstream launcher_log;

static void llog(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    std::string msg = vformat(fmt, ap);
    va_end(ap);
    std::cout << msg;
    if (launcher_log.is_open()) {
        launcher_log << msg;
        launcher_log.flush();
    }
}

static void llog_init() {
}


PROCESS_INFORMATION CreateMhoProcess(
        const std::wstring &mho_dir,
        const std::wstring &mho_exe,
        const std::wstring &mho_arg,
        const std::wstring &work_dir) {
    STARTUPINFOW si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    si.lpTitle = L"IIPSMsgWnd";
    si.wShowWindow = SW_NORMAL;
    si.dwFlags |= STARTF_USESHOWWINDOW;

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    std::wstring cmdStr = mho_dir + mho_exe + L" " + mho_arg;
    llog("Creating process: \"%s\"\n", ws_2_s(cmdStr).c_str());
    llog("Working dir: \"%s\"\n", ws_2_s(work_dir).c_str());

    int ret = CreateProcessW(NULL,
                             const_cast<wchar_t *>(cmdStr.c_str()),
                             NULL, NULL, FALSE,
                             CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS,
                             NULL, work_dir.c_str(), &si, &pi);
    if (ret == FALSE) {
        DWORD err = GetLastError();
        llog("CreateProcess failed (%lu) Msg:%s\n", err, GetLastErrorAsString(err).c_str());
        memset(&pi, 0, sizeof(pi));
        return pi;
    }
    llog("Created Process Success (PID:%lu)\n", pi.dwProcessId);
    return pi;
}


PROCESS_INFORMATION CreateMhoProcessOrg(
        const std::wstring &mho_dir,
        const std::wstring &mho_exe,
        const std::wstring &mho_arg,
        const std::wstring &work_dir) {
    STARTUPINFOW si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    si.lpTitle = L"IIPSMsgWnd";
    si.wShowWindow = SW_NORMAL;
    si.dwFlags |= STARTF_USESHOWWINDOW;

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    std::wstring cmdStr = mho_dir + mho_exe + L" " + mho_arg;
    llog("Creating process (org): \"%s\"\n", ws_2_s(cmdStr).c_str());

    int ret = CreateProcessW(NULL,
                             const_cast<wchar_t *>(cmdStr.c_str()),
                             NULL, NULL, FALSE,
                             CREATE_UNICODE_ENVIRONMENT,
                             NULL, work_dir.c_str(), &si, &pi);
    if (ret == FALSE) {
        DWORD err = GetLastError();
        llog("CreateProcess failed (%lu) Msg:%s\n", err, GetLastErrorAsString(err).c_str());
        memset(&pi, 0, sizeof(pi));
        return pi;
    }
    llog("Created Process Success (PID:%lu)\n", pi.dwProcessId);
    return pi;
}

void inject_dll(PROCESS_INFORMATION pi, const std::wstring &dll_path) {
    if (pi.hProcess == NULL) {
        llog("inject_dll: skipped, no process\n");
        return;
    }

    llog("Injecting DLL: \"%s\"\n", ws_2_s(dll_path).c_str());

    const wchar_t *lib_path = dll_path.c_str();
    SIZE_T lib_path_size = (wcslen(lib_path) + 1) * sizeof(wchar_t);
    void *lib_base_address = VirtualAllocEx(
            pi.hProcess, NULL, lib_path_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!lib_base_address) {
        llog("VirtualAllocEx failed (%lu)\n", GetLastError());
        return;
    }

    WriteProcessMemory(pi.hProcess, lib_base_address, (void *) lib_path, lib_path_size, NULL);

    LPTHREAD_START_ROUTINE load_library_proc_address = (LPTHREAD_START_ROUTINE) GetProcAddress(
            GetModuleHandleA("Kernel32"), "LoadLibraryW");

    HANDLE load_library_thread = CreateRemoteThread(
            pi.hProcess, NULL, 0, load_library_proc_address, lib_base_address, 0, NULL);
    if (load_library_thread == NULL) {
        llog("CreateRemoteThread failed (%lu)\n", GetLastError());
        return;
    }

    // LoadLibraryW's return (the HMODULE, or NULL on failure) becomes the
    // remote thread's exit code. Wait for it and check — CreateRemoteThread
    // returning a valid handle only means the thread started, not that the
    // DLL actually loaded (e.g. wrong bitness, missing dep).
    WaitForSingleObject(load_library_thread, INFINITE);
    DWORD remote_hmodule = 0;
    GetExitCodeThread(load_library_thread, &remote_hmodule);
    CloseHandle(load_library_thread);

    if (remote_hmodule == 0) {
        llog("DLL injection FAILED (LoadLibraryW returned NULL): \"%s\"\n",
             ws_2_s(dll_path).c_str());
        return;
    }
    llog("DLL injected successfully: \"%s\" (HMODULE=0x%08lX)\n",
         ws_2_s(dll_path).c_str(), remote_hmodule);
}

void inject_lunch(PROCESS_INFORMATION pi) {
    inject_dll(pi, get_exe_dir() + L"ag_mho.dll");
}

void inject_haunt(PROCESS_INFORMATION pi) {
    inject_dll(pi, get_exe_dir() + L"haunt-x86.dll");
}

int main(int argc, char *argv[]) {

    std::wstring mho_dir;
    std::wstring mho_exe;
    std::wstring mho_arg;
    std::wstring work_dir;

    if (argc == 2) {
        mho_dir = get_exe_dir();
        mho_exe = L"MHOClient.exe";
        mho_arg = s_2_ws(argv[1]);
    } else if (argc == 4) {
        mho_dir = s_2_ws(argv[1]);
        mho_exe = s_2_ws(argv[2]);
        mho_arg = s_2_ws(argv[3]);
    } else {
        mho_dir = get_exe_dir();
        mho_exe = L"MHOClient.exe";
        mho_arg = L"-q 123456789 -src=tgp -game_id 45 -area 1 -zone_id 17306122 -nosplash";
    }
    work_dir = mho_dir;

    llog_init();
    llog("MHO Launcher\n");
    llog("exe_dir: \"%s\"\n", ws_2_s(get_exe_dir()).c_str());
    llog("mho_dir: \"%s\"\n", ws_2_s(mho_dir).c_str());
    llog("mho_exe: \"%s\"\n", ws_2_s(mho_exe).c_str());

    std::wstring ini_path = get_exe_dir() + L"ag_mho.ini";
    if (ag_ini_create_if_missing(ini_path, AG_MHO_INI_DEFAULTS)) {
        llog("Created default config: \"%s\"\n", ws_2_s(ini_path).c_str());
    }
    auto ag_cfg = ag_ini_read(ini_path);
    int cfg_inject_dll = ag_ini_get_int(ag_cfg, "inject_dll", 0);
    int cfg_inject_haunt = ag_ini_get_int(ag_cfg, "inject_haunt", 0);
    llog("config inject_dll = %d\n", cfg_inject_dll);
    llog("config inject_haunt = %d\n", cfg_inject_haunt);

    if (!cfg_inject_dll) {
        PROCESS_INFORMATION pi = CreateMhoProcessOrg(mho_dir, mho_exe, mho_arg, work_dir);
        if (cfg_inject_haunt) {
            inject_haunt(pi);
        }
        TenProxyTclsSharedMeMemory *tptsmm = new TenProxyTclsSharedMeMemory();
        tptsmm->map(pi.dwProcessId);
        do {
            std::cout << '\n' << "Press a key to exit...";
        } while (std::cin.get() != '\n');
    } else {
        PROCESS_INFORMATION pi = CreateMhoProcess(mho_dir, mho_exe, mho_arg, work_dir);
        inject_lunch(pi);
        if (cfg_inject_haunt) {
            inject_haunt(pi);
        }
        do {
            std::cout << '\n' << "Press a key to resume MHOClient.exe...";
        } while (std::cin.get() != '\n');
        ResumeThread(pi.hThread);
    }

    return 0;
}
