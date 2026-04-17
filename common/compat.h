#ifndef MHO_COMPAT_H
#define MHO_COMPAT_H

#ifdef __MINGW32__

#include <cstdio>
#include <cerrno>
#include <process.h>

inline errno_t freopen_s(FILE** pFile, const char* filename, const char* mode, FILE* stream) {
    FILE* f = freopen(filename, mode, stream);
    if (pFile) *pFile = f;
    return (f == nullptr) ? errno : 0;
}

#endif

#endif
