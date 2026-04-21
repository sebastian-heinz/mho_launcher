#ifndef MHO_HOOKS_MHOCLIENT_H
#define MHO_HOOKS_MHOCLIENT_H

#include <windows.h>
#include <cstdint>

extern DWORD server_url_address;

void install_mhoclient_hooks(DWORD mhoclient_base);

#endif
