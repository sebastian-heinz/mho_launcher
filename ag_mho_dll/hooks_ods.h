#ifndef MHO_HOOKS_ODS_H
#define MHO_HOOKS_ODS_H

// Inline-detour OutputDebugStringA/W in kernel32 so any process-wide
// OutputDebugString call (ours, haunt.dll, the CRT, the game) is routed
// into our log pipeline and shows up in the allocated console + log file.
void install_ods_hooks();

#endif
