#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>

#pragma comment(lib, "dbghelp")

static LPTOP_LEVEL_EXCEPTION_FILTER g_prev_filter = NULL;

static void get_dump_path(char *buf, size_t buf_size) {
    char module_path[MAX_PATH];
    GetModuleFileNameA(NULL, module_path, MAX_PATH);

    char *last_slash = strrchr(module_path, '\\');
    if (last_slash) *(last_slash + 1) = '\0';

    SYSTEMTIME st;
    GetLocalTime(&st);
    snprintf(buf, buf_size,
             "%sCrashDumps\\crash_%04d%02d%02d_%02d%02d%02d_%lu.dmp",
             module_path,
             st.wYear, st.wMonth, st.wDay,
             st.wHour, st.wMinute, st.wSecond,
             GetCurrentProcessId());
}

static void ensure_crashdumps_dir(void) {
    char module_path[MAX_PATH];
    GetModuleFileNameA(NULL, module_path, MAX_PATH);
    char *last_slash = strrchr(module_path, '\\');
    if (last_slash) *(last_slash + 1) = '\0';
    strcat(module_path, "CrashDumps");
    CreateDirectoryA(module_path, NULL);
}

static LONG WINAPI crash_handler(EXCEPTION_POINTERS *ep) {
    ensure_crashdumps_dir();

    char dump_path[MAX_PATH];
    get_dump_path(dump_path, sizeof(dump_path));

    HANDLE file = CreateFileA(dump_path, GENERIC_WRITE, 0, NULL,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION mei;
        mei.ThreadId = GetCurrentThreadId();
        mei.ExceptionPointers = ep;
        mei.ClientPointers = FALSE;

        MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            file,
            MiniDumpWithFullMemory |
            MiniDumpWithHandleData |
            MiniDumpWithThreadInfo |
            MiniDumpWithUnloadedModules,
            &mei,
            NULL,
            NULL);

        CloseHandle(file);
    }

    if (g_prev_filter)
        return g_prev_filter(ep);

    return EXCEPTION_CONTINUE_SEARCH;
}

/* ---- Exported functions (match original BugTrace.dll) ---- */

__declspec(dllexport) void __cdecl InitializeBugTrace(void) {
}

__declspec(dllexport) void __cdecl WriteUserDataToDump(void) {
}

__declspec(dllexport) void __cdecl CreateUploadBOSDDumpThread(void) {
}

__declspec(dllexport) void __cdecl SetBOSDDumpInfo(void) {
}

__declspec(dllexport) void __cdecl UploadBOSDDump(void) {
}

/* ---- DLL entry ---- */

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        g_prev_filter = SetUnhandledExceptionFilter(crash_handler);
    }
    return TRUE;
}
