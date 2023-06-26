#ifndef MHO_LAUNCHER_MEMORY_H
#define MHO_LAUNCHER_MEMORY_H

#include <windows.h>

template<typename I>
void GetMemory(LPVOID address, I &value, int byteNum) {
    unsigned long OldProtection;
    VirtualProtect(address, byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);
    memcpy(static_cast<void *>(value), address, byteNum);
    VirtualProtect(address, byteNum, OldProtection, &OldProtection);
}

template<typename I>
void SetMemory(LPVOID address, I value, int byteNum) {
    unsigned long OldProtection;
    VirtualProtect(address, byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);
    *(I *) address = value;
    VirtualProtect(address, byteNum, OldProtection, &OldProtection);
}

void ReadMemory(LPVOID address, void *value, int byteNum) {
    unsigned long OldProtection;
    VirtualProtect(address, byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);
    memcpy(value, address, byteNum);
    VirtualProtect(address, byteNum, OldProtection, &OldProtection);
}

template<typename I>
void WriteMemory(LPVOID address, I value, int byteNum) {
    unsigned long OldProtection;
    VirtualProtect(address, byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);
    memcpy(address, value, byteNum);
    VirtualProtect(address, byteNum, OldProtection, &OldProtection);
}

void hook_call(DWORD baseAddr, DWORD offset, LPVOID fnAddr) {
    DWORD patchHookAddr = baseAddr + offset;
    DWORD relativeFnHookAddr = (DWORD) ((char *) fnAddr - (char *) (patchHookAddr + 1 + 4));
    const char *patchInitStart = "\xE8";
    WriteMemory((LPVOID) patchHookAddr, patchInitStart, 1);
    BYTE bRelativeHookInitAddr[4];
    memcpy(bRelativeHookInitAddr, &relativeFnHookAddr, 4);
    WriteMemory((LPVOID) (patchHookAddr + 1), bRelativeHookInitAddr, 4);
}

void patch_jmp(DWORD baseAddr, DWORD offset, LPVOID fnAddr) {
    DWORD patchHookAddr = baseAddr + offset;
    DWORD relativeFnHookAddr = (DWORD) ((char *) fnAddr - (char *) (patchHookAddr + 1 + 4));
    const char *patchInitStart = "\xE9";
    WriteMemory((LPVOID) patchHookAddr, patchInitStart, 1);
    BYTE bRelativeHookInitAddr[4];
    memcpy(bRelativeHookInitAddr, &relativeFnHookAddr, 4);
    WriteMemory((LPVOID) (patchHookAddr + 1), bRelativeHookInitAddr, 4);
}

void patch_nop(DWORD baseAddr, DWORD offset, int count) {
    DWORD patchHookAddr = baseAddr + offset;
    char *nops = new char[count];
    for (int i = 0; i < count; i++) {
        nops[i] = (char) 0x90;
    }
    WriteMemory((LPVOID) patchHookAddr, nops, count);
    delete[] nops;
}

#endif //MHO_LAUNCHER_MEMORY_H
