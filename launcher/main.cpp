#include "win_util.h"
#include "str_util.h"
#include "shared_memory.h"

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
    time_t now = time(nullptr);
    struct tm *t = localtime(&now);
    char timestamp[64];
    snprintf(timestamp, sizeof(timestamp), "mho_launcher_exe_%04d%02d%02d_%02d%02d%02d.log",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
    std::string path = ws_2_s(get_exe_dir()) + timestamp;
    launcher_log.open(path, std::ios::out | std::ios::trunc);
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

void inject_lunch(PROCESS_INFORMATION pi) {
    if (pi.hProcess == NULL) {
        llog("inject_lunch: skipped, no process\n");
        return;
    }

    std::wstring mho_launcher_lib_path = get_exe_dir() + L"mho_launcher_lib.dll";
    llog("Injecting DLL: \"%s\"\n", ws_2_s(mho_launcher_lib_path).c_str());

    const wchar_t *lib_path = mho_launcher_lib_path.c_str();
    SIZE_T lib_path_size = wcslen(lib_path) * sizeof(wchar_t);
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
    llog("DLL injected successfully\n");
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

    if (false) {
        PROCESS_INFORMATION pi = CreateMhoProcessOrg(mho_dir, mho_exe, mho_arg, work_dir);
        TenProxyTclsSharedMeMemory *tptsmm = new TenProxyTclsSharedMeMemory();
        tptsmm->map(pi.dwProcessId);
        do {
            std::cout << '\n' << "Press a key to exit...";
        } while (std::cin.get() != '\n');
    } else {
        PROCESS_INFORMATION pi = CreateMhoProcess(mho_dir, mho_exe, mho_arg, work_dir);
        inject_lunch(pi);
        do {
            std::cout << '\n' << "Press a key to resume MHOClient.exe...";
        } while (std::cin.get() != '\n');
        ResumeThread(pi.hThread);
    }

    return 0;
}
