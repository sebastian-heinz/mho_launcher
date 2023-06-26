#ifndef MHO_LAUNCHER_UTIL_H
#define MHO_LAUNCHER_UTIL_H

#include <windows.h>
#include <string>
#include <codecvt>
#include <vector>

std::wstring get_exe_path() {
    WCHAR exePath[MAX_PATH + 1];
    DWORD pathLen = GetModuleFileNameW(NULL, exePath, MAX_PATH);
    if (pathLen <= 0) {
        return NULL;
    }
    std::wstring path = std::wstring(exePath);
    return path;
}

std::wstring get_exe_dir() {
    std::wstring path = get_exe_path();
    size_t idx = path.find_last_of(L"/\\");
    if (idx == std::wstring::npos) {
        return NULL;
    }
    idx++;
    size_t len = path.length();
    if (idx >= len) {
        return NULL;
    }
    std::wstring dir = path.substr(0, idx);
    return dir;
}

std::wstring get_exe_name() {
    std::wstring path = get_exe_path();
    size_t idx = path.find_last_of(L"/\\");
    if (idx == std::wstring::npos) {
        return NULL;
    }
    idx++;
    size_t len = path.length();
    if (idx >= len) {
        return NULL;
    }
    std::wstring dir = path.substr(idx, len - idx);
    return dir;
}

std::string GetLastErrorAsString(DWORD error) {
    if (error == 0) {
        return std::string(); //No error message has been recorded
    }
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR) &messageBuffer,
            0,
            NULL
    );
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}

std::string to_hex(uint8_t *bytes, int size) {
    static const char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    std::string str;
    for (int i = 0; i < size; ++i) {
        const char ch = bytes[i];
        str.append(&hex[(ch & 0xF0) >> 4], 1);
        str.append(&hex[ch & 0xF], 1);
        str.append(" ");
    }
    return str;
}

std::string to_ascii(uint8_t *bytes, int size) {
    std::string str;
    for (int i = 0; i < size; ++i) {
        const char ch = bytes[i];
        if (ch >= 32 && ch <= 127) {
            str.append(&ch, 1);
        } else {
            str.append(".");
        }
    }
    return str;
}

void show(uint8_t *bytes, int size) {
    fprintf(stdout, "\n");
    fprintf(stdout, "---------\n");
    fprintf(stdout, "Ptr:%p Size:%d\n", bytes, size);

    int chunk_size = 16;
    int chunks = size / chunk_size;
    int rem = size % chunk_size;

    int offset = 0;
    for (int i = 0; i < chunks; i++) {
        offset = i * chunk_size;
        fprintf(stdout, "0x%08X | %s| %s\n",
                offset,
                to_hex(&bytes[offset], chunk_size).c_str(),
                to_ascii(&bytes[offset], chunk_size).c_str()
        );
    }
    if (rem > 0) {
        offset = chunks * chunk_size;
        fprintf(stdout, "0x%08X | %s| %s\n",
                offset,
                to_hex(&bytes[offset], rem).c_str(),
                to_ascii(&bytes[offset], rem).c_str()
        );
    }

    fprintf(stdout, "---------\n");
    fprintf(stdout, "\n");
}

std::wstring s_2_ws(const std::string &str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

std::string ws_2_s(const std::wstring &wstr) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

std::string get_utf8(const std::wstring &wstr)
{
    if (wstr.empty()) return std::string();
    int sz = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], -1, 0, 0, 0, 0);
    std::string res(sz, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], -1, &res[0], sz, 0, 0);
    return res;
}

std::wstring get_utf16(const std::string &str)
{
    if (str.empty()) return std::wstring();
    int sz = MultiByteToWideChar(CP_UTF8, 0, &str[0], -1, 0, 0);
    std::wstring res(sz, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], -1, &res[0], sz);
    return res;
}

// https://stackoverflow.com/a/69911
std::string vformat(const char *fmt, va_list ap) {
    // Allocate a buffer on the stack that's big enough for us almost
    // all the time.  Be prepared to allocate dynamically if it doesn't fit.
    size_t size = 1024;
    char stackbuf[1024];
    std::vector<char> dynamicbuf;
    char *buf = &stackbuf[0];
    va_list ap_copy;

    while (true) {
        // Try to vsnprintf into our buffer.
        va_copy(ap_copy, ap);
        int needed = vsnprintf(buf, size, fmt, ap);
                va_end(ap_copy);

        // NB. C99 (which modern Linux and OS X follow) says vsnprintf
        // failure returns the length it would have needed.  But older
        // glibc and current Windows return -1 for failure, i.e., not
        // telling us how much was needed.

        if (needed <= (int) size && needed >= 0) {
            // It fit fine so we're done.
            return std::string(buf, (size_t) needed);
        }

        // vsnprintf reported that it wanted to write more characters
        // than we allotted.  So try again using a dynamic buffer.  This
        // doesn't happen very often if we chose our initial size well.
        size = (needed > 0) ? (needed + 1) : (size * 2);
        dynamicbuf.resize(size);
        buf = &dynamicbuf[0];
    }
}

std::string format(const char *fmt, ...) {
    va_list ap;
            va_start (ap, fmt);
    std::string buf = vformat(fmt, ap);
            va_end (ap);
    return buf;
}

uint8_t *map_memory(std::string p_map_name, size_t p_map_size) {
    HANDLE shared_mem_handle = CreateFileMappingA(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            p_map_size,
            p_map_name.c_str()
    );
    if (!shared_mem_handle) {
        DWORD err = GetLastError();
        printf("CreateFileMappingA failed (%lu) Msg:%s\n", err, GetLastErrorAsString(err).c_str());
        return nullptr;
    }

    void *shared_mem = MapViewOfFile(
            shared_mem_handle,
            FILE_MAP_WRITE,
            0, 0,
            0
    );
    if (!shared_mem) {
        DWORD err = GetLastError();
        printf("MapViewOfFile failed (%lu) Msg:%s\n", err, GetLastErrorAsString(err).c_str());
        return nullptr;
    }
    printf("MapViewOfFile:%s\n", p_map_name.c_str());

    return (uint8_t *) shared_mem;
}

#endif //MHO_LAUNCHER_UTIL_H
