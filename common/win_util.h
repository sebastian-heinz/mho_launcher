#ifndef MHO_WIN_UTIL_H
#define MHO_WIN_UTIL_H

#include <windows.h>
#include <cstdint>
#include <cstdarg>
#include <string>
#include <vector>

inline std::wstring get_exe_path() {
    WCHAR exePath[MAX_PATH + 1];
    DWORD pathLen = GetModuleFileNameW(NULL, exePath, MAX_PATH);
    if (pathLen <= 0) {
        return std::wstring();
    }
    return std::wstring(exePath);
}

inline std::wstring get_exe_dir() {
    std::wstring path = get_exe_path();
    size_t idx = path.find_last_of(L"/\\");
    if (idx == std::wstring::npos) {
        return std::wstring();
    }
    return path.substr(0, idx + 1);
}

inline std::wstring get_exe_name() {
    std::wstring path = get_exe_path();
    size_t idx = path.find_last_of(L"/\\");
    if (idx == std::wstring::npos) {
        return std::wstring();
    }
    return path.substr(idx + 1);
}

inline std::string GetLastErrorAsString(DWORD error) {
    if (error == 0) {
        return std::string();
    }
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR) &messageBuffer, 0, NULL);
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}

inline uint8_t *map_memory(std::string p_map_name, size_t p_map_size) {
    HANDLE shared_mem_handle = CreateFileMappingA(
            INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
            0, p_map_size, p_map_name.c_str());
    if (!shared_mem_handle) {
        DWORD err = GetLastError();
        printf("CreateFileMappingA failed (%lu) Msg:%s\n", err, GetLastErrorAsString(err).c_str());
        return nullptr;
    }
    void *shared_mem = MapViewOfFile(shared_mem_handle, FILE_MAP_WRITE, 0, 0, 0);
    if (!shared_mem) {
        DWORD err = GetLastError();
        printf("MapViewOfFile failed (%lu) Msg:%s\n", err, GetLastErrorAsString(err).c_str());
        return nullptr;
    }
    printf("MapViewOfFile:%s\n", p_map_name.c_str());
    return (uint8_t *) shared_mem;
}

inline std::string vformat(const char *fmt, va_list ap) {
    size_t size = 1024;
    char stackbuf[1024];
    std::vector<char> dynamicbuf;
    char *buf = &stackbuf[0];
    va_list ap_copy;

    while (true) {
        va_copy(ap_copy, ap);
        int needed = vsnprintf(buf, size, fmt, ap);
        va_end(ap_copy);

        if (needed <= (int) size && needed >= 0) {
            return std::string(buf, (size_t) needed);
        }
        size = (needed > 0) ? (needed + 1) : (size * 2);
        dynamicbuf.resize(size);
        buf = &dynamicbuf[0];
    }
}

inline std::string format(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    std::string buf = vformat(fmt, ap);
    va_end(ap);
    return buf;
}

#endif
