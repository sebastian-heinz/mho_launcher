#ifndef MHO_BACKTRACE_UTIL_H
#define MHO_BACKTRACE_UTIL_H

struct HookContext;

// Walk the EBP frame-pointer chain starting from `ctx` (a JmpHookEntry trampoline
// context). Each frame is emitted on its own line as:
//
//     [<tag>.bt] f<i> ret=0x<addr> <module>+0x<offset>
//
// Stops early when ebp goes out of range, ret addr is unreadable / not code-like,
// or ebp doesn't strictly grow upward (FPO / garbage frame).
//
// Used vs CaptureStackBackTrace: ebp-walk is safer inside JmpHook trampolines
// because it doesn't go through RtlVirtualUnwind on Wine (which can hang/crash
// when our generated trampoline lacks .pdata unwind info).
void log_caller_stack(HookContext *ctx, const char *tag, int max_depth);

#endif
