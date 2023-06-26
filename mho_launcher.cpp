#include "util.h"
#include "tenproxy_tcls_sharedmememory.h"

#include <windows.h>
#include <iostream>
#include <string>


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

    std::wstring application_name = mho_dir + mho_exe;
    std::wstring cmd_args = mho_exe + L" " + mho_arg;

    fwprintf(stdout, L"Creating process: \"%s%s %s\"\n", mho_dir.c_str(), mho_exe.c_str(), mho_arg.c_str());
    std::wstring cmdStr = mho_dir + mho_exe + L" " + mho_arg;
    int ret = CreateProcessW(NULL,
                             const_cast<wchar_t *>(cmdStr.c_str()),
                             NULL,
                             NULL,
                             FALSE,
                             CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_PROCESS_GROUP |
                             DETACHED_PROCESS,
                             NULL,
                             work_dir.c_str(),
                             &si,
                             &pi);
    if (ret == FALSE) {
        DWORD err = GetLastError();
        printf("CreateProcess failed (%lu) Msg:%s\n", err, GetLastErrorAsString(err).c_str());
        memset(&pi, 0, sizeof(pi));
        return pi;
    }
    fprintf(stdout, "Created Process Success (%lu)\n", pi.dwProcessId);
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

    std::wstring application_name = mho_dir + mho_exe;
    std::wstring cmd_args = mho_exe + L" " + mho_arg;

    fwprintf(stdout, L"Creating process: \"%s%s %s\"\n", mho_dir.c_str(), mho_exe.c_str(), mho_arg.c_str());
    std::wstring cmdStr = mho_dir + mho_exe + L" " + mho_arg;
    int ret = CreateProcessW(NULL,
                             const_cast<wchar_t *>(cmdStr.c_str()),
                             NULL,
                             NULL,
                             FALSE,
                             CREATE_UNICODE_ENVIRONMENT,
                             NULL,
                             work_dir.c_str(),
                             &si,
                             &pi);
    if (ret == FALSE) {
        DWORD err = GetLastError();
        printf("CreateProcess failed (%lu) Msg:%s\n", err, GetLastErrorAsString(err).c_str());
        memset(&pi, 0, sizeof(pi));
        return pi;
    }
    fprintf(stdout, "Created Process Success (%lu)\n", pi.dwProcessId);
    return pi;
}

void inject_lunch(PROCESS_INFORMATION pi) {
    // Allocate memory in the process
    std::wstring mho_launcher_lib_path = get_exe_dir() + L"mho_launcher_lib.dll";
    const wchar_t *lib_path = mho_launcher_lib_path.c_str();
    SIZE_T lib_path_size = wcslen(lib_path) * sizeof(wchar_t);
    void *lib_base_address = VirtualAllocEx(
            pi.hProcess,
            NULL,
            lib_path_size,
            MEM_COMMIT,
            PAGE_EXECUTE_READWRITE
    );

    // Write path of mho_launcher_lib.dll into process memory
    WriteProcessMemory(
            pi.hProcess,
            lib_base_address,
            (void *) lib_path,
            lib_path_size,
            NULL
    );

    // Retrieve address for LoadLibraryW function
    LPTHREAD_START_ROUTINE load_library_proc_address = (LPTHREAD_START_ROUTINE) GetProcAddress(
            GetModuleHandleA("Kernel32"),
            "LoadLibraryW"
    );

    // Create a new thread in the process that calls LoadLibraryW with allocated memory that contains the path of mho_launcher_lib.dll as parameter
    HANDLE load_library_thread = CreateRemoteThread(
            pi.hProcess,
            NULL,
            0,
            load_library_proc_address,
            lib_base_address,
            0,
            NULL
    );
    if (load_library_thread == NULL) {
        fprintf(stderr, "load_library_thread == NULL\n");
        return;
    }


}

int main(int argc, char *argv[]) {

    std::cout << "MHO Launcher" << std::endl;

    std::wstring mho_dir;
    std::wstring mho_exe;
    std::wstring mho_arg;
    std::wstring work_dir;

// @formatter:off
    std::cout << "Usage:" << std::endl;
    std::cout << "1) no args, runs MHOClient.exe in same directory with fixed args" << std::endl;
    std::cout << "1) ->mho_launcher.exe" << std::endl;
    std::cout << "2) args, runs MHOClient.exe in same directory with provided args" << std::endl;
    std::cout << "2) ->mho_launcher.exe {args}" << std::endl;
    std::cout << "2) ->ex: mho_launcher.exe \"-q 123456789 -src=tgp -game_id 45 -area 1 -zone_id 17306122 -nosplash\"" << std::endl;
    std::cout << "3) runs specified executable in specified dir with specified arguments" << std::endl;
    std::cout << "3) ->mho_launcher.exe {dir} {exe} {arg}" << std::endl;
    std::cout << "3) ->ex: mho_launcher.exe \"D:\\games\\Monster Hunter Online\\Bin\\Client\\Bin32\" \"MHOClient.exe\" \"-q 123456789 -src=tgp -game_id 45 -area 1 -zone_id 17306122 -nosplash\"" << std::endl;
// @formatter:on

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



    if (false) {
        // this is to lunch any .exe without any injection or mods and make the memory map available
        PROCESS_INFORMATION pi = CreateMhoProcessOrg(mho_dir, mho_exe, mho_arg, work_dir);
        TenProxyTclsSharedMeMemory *tptsmm = new TenProxyTclsSharedMeMemory();
        tptsmm->map(pi.dwProcessId);
        do {
            std::cout << '\n' << "Press a key to exit...";
        } while (std::cin.get() != '\n');
    } else {
        // this is intended to be lunched with the unpacked .exe, it hooks logging fns
        PROCESS_INFORMATION pi = CreateMhoProcess(mho_dir, mho_exe, mho_arg, work_dir);
        inject_lunch(pi);
        do {
            std::cout << '\n' << "Press a key to resume MHOClient.exe...";
        } while (std::cin.get() != '\n');
        ResumeThread(pi.hThread);
    }


    return 0;
}
