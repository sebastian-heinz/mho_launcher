#ifndef MHO_STR_UTIL_H
#define MHO_STR_UTIL_H

#include <windows.h>
#include <string>

#ifndef __MINGW32__
#include <codecvt>
#endif

inline std::string get_utf8(const std::wstring &wstr) {
    if (wstr.empty()) return std::string();
    int sz = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), 0, 0, 0, 0);
    std::string res(sz, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &res[0], sz, 0, 0);
    return res;
}

inline std::wstring get_utf16(const std::string &str) {
    if (str.empty()) return std::wstring();
    int sz = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), 0, 0);
    std::wstring res(sz, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &res[0], sz);
    return res;
}

#ifdef __MINGW32__

inline std::wstring s_2_ws(const std::string &str) {
    return get_utf16(str);
}

inline std::string ws_2_s(const std::wstring &wstr) {
    return get_utf8(wstr);
}

#else

inline std::wstring s_2_ws(const std::string &str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.from_bytes(str);
}

inline std::string ws_2_s(const std::wstring &wstr) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.to_bytes(wstr);
}

#endif

#endif
