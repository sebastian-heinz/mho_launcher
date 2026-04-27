#ifndef MHO_HOOKS_MHO_HANDLERS_H
#define MHO_HOOKS_MHO_HANDLERS_H

#include <windows.h>

// Installs JmpHookEntry hooks on every MHOClient.exe handler-applicator function
// auto-discovered by the capstone scan (functions in range 0x880000..0x900000
// that contain `call [reg+0x20]` = CCliLogicMonster vt[8] SetPosition).
//
// Each hook fires at function entry and emits a [MhoHandler] log line with the
// `this`, vtable, first 4 stack args, and caller return address. The callback
// filters to only log when `this` points to a CCliLogicMonster.
void install_mho_handler_hooks(DWORD mhoclient_base);

#endif
