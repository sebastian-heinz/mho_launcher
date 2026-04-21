#include "dump_module.h"
#include "log.h"
#include "hook_table.h"
#include "str_util.h"
#include "win_util.h"

#include <windows.h>
#include <tlhelp32.h>
#include <cstdio>
#include <cstring>
#include <string>

static void enumerate_modules(void) {
    log("dump_module: enumerating loaded modules:\n");
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 me;
        me.dwSize = sizeof(me);
        if (Module32First(snap, &me)) {
            do {
                log("  module: %s at 0x%08X (size=0x%X)\n",
                    me.szModule, (DWORD)me.modBaseAddr, me.modBaseSize);
            } while (Module32Next(snap, &me));
        }
        CloseHandle(snap);
    }
}

static DWORD find_module(const char *module_name) {
    const char *variants[] = {
        module_name,
        "MHOClientBase.dll",
        "MHOClientBase",
        "mhoclientbase.dll",
        "mhoclientbase",
        "MHOClientbase.dll",
        NULL
    };

    for (int attempt = 0; attempt < 300; attempt++) {
        for (int v = 0; variants[v]; v++) {
            HMODULE h = GetModuleHandleA(variants[v]);
            if (h) {
                log("dump_module: found '%s' (variant '%s') at 0x%08X\n",
                    module_name, variants[v], (DWORD)h);
                return (DWORD)h;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    log("dump_module: '%s' not found after 30s\n", module_name);
    enumerate_modules();
    return 0;
}

static void dump_pe(const char *module_name, const char *out_filename) {
    DWORD base = find_module(module_name);
    if (!base) return;

    log("dump_module: waiting 10s for Themida unpack...\n");
    std::this_thread::sleep_for(std::chrono::seconds(10));

    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        log("dump_module: bad DOS signature at 0x%08X\n", base);
        return;
    }

    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        log("dump_module: bad NT signature\n");
        return;
    }

    DWORD size_of_headers = nt->OptionalHeader.SizeOfHeaders;
    WORD num_sections = nt->FileHeader.NumberOfSections;

    log("dump_module: SizeOfImage=0x%X, Sections=%d\n",
        nt->OptionalHeader.SizeOfImage, num_sections);

    std::wstring exe_dir = get_exe_dir();
    std::string dir = ws_2_s(exe_dir);
    std::string path = dir + out_filename;

    FILE *f = fopen(path.c_str(), "wb");
    if (!f) {
        log("dump_module: failed to open %s for writing\n", path.c_str());
        return;
    }

    fwrite((void *)base, 1, size_of_headers, f);

    IMAGE_SECTION_HEADER *sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < num_sections; i++) {
        char name[9] = {};
        memcpy(name, sec[i].Name, 8);
        DWORD va = sec[i].VirtualAddress;
        DWORD raw_size = sec[i].SizeOfRawData;
        DWORD raw_off = sec[i].PointerToRawData;
        DWORD vsize = sec[i].Misc.VirtualSize;
        DWORD write_size = (raw_size > 0) ? raw_size : vsize;

        log("dump_module: section %s VA=0x%X raw_off=0x%X size=0x%X\n",
            name, va, raw_off, write_size);

        fseek(f, raw_off, SEEK_SET);
        void *src = (void *)(base + va);

        /* Only dump safe sections — skip .tvm0/.tls to avoid
           Themida anti-tamper crash */
        if (strncmp(name, ".tvm", 4) == 0 || strcmp(name, ".tls") == 0) {
            log("dump_module: section %s SKIPPED (Themida)\n", name);
            char *zeros = (char *)calloc(1, write_size);
            fwrite(zeros, 1, write_size, f);
            free(zeros);
        } else {
            fwrite(src, 1, write_size, f);
        }
    }

    fclose(f);
    log("dump_module: wrote %s (%d sections)\n", path.c_str(), num_sections);
}

void install_module_dumper() {
    /* MHOClientBase is never loaded as a DLL module (manually mapped or embedded in exe).
       Dump tersafe.dll and MHOClient.exe instead — those contain the relevant code. */
    log("dump_module: waiting for tersafe...\n");
    wait_for_module("tersafe");
    log("dump_module: tersafe found, waiting 10s for Themida unpack...\n");
    std::this_thread::sleep_for(std::chrono::seconds(10));
    enumerate_modules();
    dump_pe("tersafe", "tersafe_dumped.dll");
    dump_pe("MHOClient.exe", "MHOClient_dumped.exe");
}
