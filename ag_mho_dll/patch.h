#ifndef MHO_PATCH_H
#define MHO_PATCH_H

#include <windows.h>
#include <cstring>

template<typename I>
inline void GetMemory(LPVOID address, I &value, int byteNum) {
    unsigned long OldProtection;
    VirtualProtect(address, byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);
    memcpy(static_cast<void *>(value), address, byteNum);
    VirtualProtect(address, byteNum, OldProtection, &OldProtection);
}

template<typename I>
inline void SetMemory(LPVOID address, I value, int byteNum) {
    unsigned long OldProtection;
    VirtualProtect(address, byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);
    *(I *) address = value;
    VirtualProtect(address, byteNum, OldProtection, &OldProtection);
}

inline void ReadMemory(LPVOID address, void *value, int byteNum) {
    unsigned long OldProtection;
    VirtualProtect(address, byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);
    memcpy(value, address, byteNum);
    VirtualProtect(address, byteNum, OldProtection, &OldProtection);
}

template<typename I>
inline void WriteMemory(LPVOID address, I value, int byteNum) {
    unsigned long OldProtection;
    VirtualProtect(address, byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);
    memcpy(address, value, byteNum);
    VirtualProtect(address, byteNum, OldProtection, &OldProtection);
}

template<typename F>
inline void hook_call(DWORD baseAddr, DWORD offset, F fnAddr) {
    DWORD patchHookAddr = baseAddr + offset;
    DWORD relativeFnHookAddr = (DWORD) ((char *) fnAddr - (char *) (patchHookAddr + 1 + 4));
    const char *patchInitStart = "\xE8";
    WriteMemory((LPVOID) patchHookAddr, patchInitStart, 1);
    BYTE bRelativeHookInitAddr[4];
    memcpy(bRelativeHookInitAddr, &relativeFnHookAddr, 4);
    WriteMemory((LPVOID) (patchHookAddr + 1), bRelativeHookInitAddr, 4);
}

template<typename F>
inline void patch_jmp(DWORD baseAddr, DWORD offset, F fnAddr) {
    DWORD patchHookAddr = baseAddr + offset;
    DWORD relativeFnHookAddr = (DWORD) ((char *) fnAddr - (char *) (patchHookAddr + 1 + 4));
    const char *patchInitStart = "\xE9";
    WriteMemory((LPVOID) patchHookAddr, patchInitStart, 1);
    BYTE bRelativeHookInitAddr[4];
    memcpy(bRelativeHookInitAddr, &relativeFnHookAddr, 4);
    WriteMemory((LPVOID) (patchHookAddr + 1), bRelativeHookInitAddr, 4);
}

inline void patch_nop(DWORD baseAddr, DWORD offset, int count) {
    DWORD patchHookAddr = baseAddr + offset;
    char *nops = new char[count];
    for (int i = 0; i < count; i++) {
        nops[i] = (char) 0x90;
    }
    WriteMemory((LPVOID) patchHookAddr, nops, count);
    delete[] nops;
}

#endif
