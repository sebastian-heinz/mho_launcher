/*
 * TerSafe.dll stub — correct calling conventions from binary analysis.
 *
 * Three vtable interfaces:
 *   1. CreateObj vtable (6 methods)  — VM-protected, sigs from caller analysis
 *   2. DllEntry vtable  (16 methods) — all thiscall, sigs from .text disasm
 *   3. Internal vtable  (14 methods) — all thiscall, sigs from .text disasm
 *
 * All methods return 0 (success) without corrupting the stack.
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static FILE *g_log = NULL;
static int g_call_count = 0;

static void ts_log(const char *fmt, ...) {
    if (!g_log) return;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(g_log, fmt, ap);
    va_end(ap);
    fflush(g_log);
}

/* ============================================================
 * Internal Object (0x260 bytes, vtable slot 3)
 * Created by DllEntry8, used by DllEntry1/DllEntry6.
 * 14 thiscall methods.
 * ============================================================ */

static void *g_int_vtable[32];
static char  g_int_obj[0x260] __attribute__((aligned(16)));

/* int_vt[0]: thiscall, 1 stack arg (init struct) */
static int __fastcall int_vt_0(void *ecx, void *edx, void *initData) {
    ts_log("[int_vt0] this=%p init=%p\n", ecx, initData);
    return 0;
}

/* int_vt[1]: thiscall, 0 stack args (shutdown/tick) */
static int __fastcall int_vt_1(void *ecx, void *edx) {
    ts_log("[int_vt1] this=%p\n", ecx);
    return 0;
}

/* int_vt[2]: thiscall, 6 stack args, ret 0x18 */
static int __fastcall int_vt_2(void *ecx, void *edx,
    int a1, int a2, int a3, int a4, int a5, int a6) {
    g_call_count++;
    ts_log("[int_vt2] this=%p a1=0x%X a2=0x%X a3=0x%X a4=0x%X a5=0x%X a6=0x%X ret=0x%X\n",
           ecx, a1, a2, a3, a4, a5, a6, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* int_vt[3]: thiscall, 5 stack args, ret 0x14 */
static int __fastcall int_vt_3(void *ecx, void *edx,
    int a1, int a2, int a3, int a4, int a5) {
    g_call_count++;
    ts_log("[int_vt3] this=%p a1=0x%X a2=0x%X a3=0x%X a4=0x%X a5=0x%X ret=0x%X\n",
           ecx, a1, a2, a3, a4, a5, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* int_vt[4]: thiscall, 4 stack args, ret 0x10 */
static int __fastcall int_vt_4(void *ecx, void *edx,
    int a1, int a2, int a3, int a4) {
    g_call_count++;
    ts_log("[int_vt4] this=%p a1=0x%X a2=0x%X a3=0x%X a4=0x%X ret=0x%X\n",
           ecx, a1, a2, a3, a4, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* int_vt[5]: thiscall, 3 stack args, ret 0x0C */
static int __fastcall int_vt_5(void *ecx, void *edx,
    int a1, int a2, int a3) {
    g_call_count++;
    ts_log("[int_vt5] this=%p a1=0x%X a2=0x%X a3=0x%X ret=0x%X\n",
           ecx, a1, a2, a3, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* int_vt[6]: thiscall, 8 stack args, ret 0x20 */
static int __fastcall int_vt_6(void *ecx, void *edx,
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) {
    g_call_count++;
    ts_log("[int_vt6] this=%p a1=0x%X a2=0x%X ... a8=0x%X ret=0x%X\n",
           ecx, a1, a2, a8, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* int_vt[7]: thiscall, 7 stack args, ret 0x1C */
static int __fastcall int_vt_7(void *ecx, void *edx,
    int a1, int a2, int a3, int a4, int a5, int a6, int a7) {
    g_call_count++;
    ts_log("[int_vt7] this=%p a1=0x%X a2=0x%X ... a7=0x%X ret=0x%X\n",
           ecx, a1, a2, a7, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* int_vt[8]: thiscall, 6 stack args, ret 0x18 */
static int __fastcall int_vt_8(void *ecx, void *edx,
    int a1, int a2, int a3, int a4, int a5, int a6) {
    g_call_count++;
    ts_log("[int_vt8] this=%p a1=0x%X a2=0x%X ... a6=0x%X ret=0x%X\n",
           ecx, a1, a2, a6, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* int_vt[9]: thiscall, 5 stack args, ret 0x14 */
static int __fastcall int_vt_9(void *ecx, void *edx,
    int a1, int a2, int a3, int a4, int a5) {
    g_call_count++;
    ts_log("[int_vt9] this=%p a1=0x%X a2=0x%X ... a5=0x%X ret=0x%X\n",
           ecx, a1, a2, a5, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* int_vt[10]: thiscall, 3 stack args, ret 0x0C */
static int __fastcall int_vt_10(void *ecx, void *edx,
    int a1, int a2, int a3) {
    g_call_count++;
    ts_log("[int_vt10] this=%p a1=0x%X a2=0x%X a3=0x%X ret=0x%X\n",
           ecx, a1, a2, a3, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* int_vt[11]: thiscall, 3 stack args, ret 0x0C */
static int __fastcall int_vt_11(void *ecx, void *edx,
    int a1, int a2, int a3) {
    g_call_count++;
    ts_log("[int_vt11] this=%p a1=0x%X a2=0x%X a3=0x%X ret=0x%X\n",
           ecx, a1, a2, a3, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* int_vt[12]: thiscall, 4 stack args, ret 0x10 */
static int __fastcall int_vt_12(void *ecx, void *edx,
    int a1, int a2, int a3, int a4) {
    g_call_count++;
    ts_log("[int_vt12] this=%p a1=0x%X a2=0x%X a3=0x%X a4=0x%X ret=0x%X\n",
           ecx, a1, a2, a3, a4, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* int_vt[13]: thiscall, 0 stack args — factory, returns sub-object ptr */
static void * __fastcall int_vt_13(void *ecx, void *edx) {
    g_call_count++;
    ts_log("[int_vt13] this=%p ret=0x%X\n", ecx, (uint32_t)__builtin_return_address(0));
    return NULL;
}

static void init_int_vtable(void) {
    g_int_vtable[0]  = int_vt_0;
    g_int_vtable[1]  = int_vt_1;
    g_int_vtable[2]  = int_vt_2;
    g_int_vtable[3]  = int_vt_3;
    g_int_vtable[4]  = int_vt_4;
    g_int_vtable[5]  = int_vt_5;
    g_int_vtable[6]  = int_vt_6;
    g_int_vtable[7]  = int_vt_7;
    g_int_vtable[8]  = int_vt_8;
    g_int_vtable[9]  = int_vt_9;
    g_int_vtable[10] = int_vt_10;
    g_int_vtable[11] = int_vt_11;
    g_int_vtable[12] = int_vt_12;
    g_int_vtable[13] = int_vt_13;
    for (int i = 14; i < 32; i++) g_int_vtable[i] = int_vt_1;
    memset(g_int_obj, 0, sizeof(g_int_obj));
    *(void **)g_int_obj = g_int_vtable;
}


/* ============================================================
 * DllEntry Object (8 bytes, vtable slot 2)
 * 16 thiscall methods.
 * ============================================================ */

static void *g_de_vtable[32];
static char  g_de_obj[16] __attribute__((aligned(16)));

/* de_vt[0]: thiscall, 1 arg, ret 4 — init */
static int __fastcall de_vt_0(void *ecx, void *edx, void *data) {
    g_call_count++;
    ts_log("[de_vt0] this=%p data=%p ret=0x%X\n", ecx, data, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[1]: thiscall, 1 arg, ret 4 — process buffer */
static int __fastcall de_vt_1(void *ecx, void *edx, void *buf) {
    g_call_count++;
    ts_log("[de_vt1] this=%p buf=%p ret=0x%X\n", ecx, buf, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[2]: thiscall, 4 args, ret 0x10 — process cmd */
static int __fastcall de_vt_2(void *ecx, void *edx,
    int cmd, int a2, int a3, void *data) {
    g_call_count++;
    ts_log("[de_vt2] this=%p cmd=0x%X a2=0x%X a3=0x%X data=%p ret=0x%X\n",
           ecx, cmd, a2, a3, data, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[3]: thiscall, 1 arg, ret 4 */
static int __fastcall de_vt_3(void *ecx, void *edx, int param) {
    g_call_count++;
    ts_log("[de_vt3] this=%p param=0x%X ret=0x%X\n", ecx, param, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[4]: thiscall, 0 args, ret — complex/tick */
static int __fastcall de_vt_4(void *ecx, void *edx) {
    g_call_count++;
    ts_log("[de_vt4] this=%p ret=0x%X\n", ecx, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[5,6,7]: thiscall, 1 arg, ret 4 — NOP stubs */
static int __fastcall de_vt_nop1(void *ecx, void *edx, int a) {
    g_call_count++;
    ts_log("[de_vt_nop] this=%p a=0x%X ret=0x%X\n", ecx, a, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[8]: thiscall, 1 arg, ret 4 — get info */
static int __fastcall de_vt_8(void *ecx, void *edx, void *outBuf) {
    g_call_count++;
    ts_log("[de_vt8] this=%p out=%p ret=0x%X\n", ecx, outBuf, (uint32_t)__builtin_return_address(0));
    if (outBuf) memset(outBuf, 0, 0x40);
    return 0;
}

/* de_vt[9]: thiscall, 2 args, ret 8 */
static int __fastcall de_vt_9(void *ecx, void *edx, int a1, int a2) {
    g_call_count++;
    ts_log("[de_vt9] this=%p a1=0x%X a2=0x%X ret=0x%X\n", ecx, a1, a2, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[10]: thiscall, 0 args, ret */
static int __fastcall de_vt_10(void *ecx, void *edx) {
    g_call_count++;
    ts_log("[de_vt10] this=%p ret=0x%X\n", ecx, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[11]: thiscall, 1 arg, ret 4 */
static int __fastcall de_vt_11(void *ecx, void *edx, void *data) {
    g_call_count++;
    ts_log("[de_vt11] this=%p data=%p ret=0x%X\n", ecx, data, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[12]: thiscall, 1 arg, ret 4 — forward */
static int __fastcall de_vt_12(void *ecx, void *edx, int param) {
    g_call_count++;
    ts_log("[de_vt12] this=%p param=0x%X ret=0x%X\n", ecx, param, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[13]: thiscall, 1 arg, ret 4 — process data */
static int __fastcall de_vt_13(void *ecx, void *edx, void *data) {
    g_call_count++;
    ts_log("[de_vt13] this=%p data=%p ret=0x%X\n", ecx, data, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[14]: thiscall, 0 args, ret */
static int __fastcall de_vt_14(void *ecx, void *edx) {
    g_call_count++;
    ts_log("[de_vt14] this=%p ret=0x%X\n", ecx, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* de_vt[15]: thiscall, 1 arg, ret 4 */
static int __fastcall de_vt_15(void *ecx, void *edx, int param) {
    g_call_count++;
    ts_log("[de_vt15] this=%p param=0x%X ret=0x%X\n", ecx, param, (uint32_t)__builtin_return_address(0));
    return 0;
}

static void init_de_vtable(void) {
    g_de_vtable[0]  = de_vt_0;
    g_de_vtable[1]  = de_vt_1;
    g_de_vtable[2]  = de_vt_2;
    g_de_vtable[3]  = de_vt_3;
    g_de_vtable[4]  = de_vt_4;
    g_de_vtable[5]  = de_vt_nop1;
    g_de_vtable[6]  = de_vt_nop1;
    g_de_vtable[7]  = de_vt_nop1;
    g_de_vtable[8]  = de_vt_8;
    g_de_vtable[9]  = de_vt_9;
    g_de_vtable[10] = de_vt_10;
    g_de_vtable[11] = de_vt_11;
    g_de_vtable[12] = de_vt_12;
    g_de_vtable[13] = de_vt_13;
    g_de_vtable[14] = de_vt_14;
    g_de_vtable[15] = de_vt_15;
    for (int i = 16; i < 32; i++) g_de_vtable[i] = de_vt_10;
    memset(g_de_obj, 0, sizeof(g_de_obj));
    *(void **)g_de_obj = g_de_vtable;
    *(uint32_t *)(g_de_obj + 4) = 0x400; /* version field */
}


/* ============================================================
 * CreateObj Object — TerSafe main interface
 * 6 methods. All VM-protected (.tvm0) in original binary.
 * Signatures determined by caller analysis + runtime testing.
 *
 * Vtable layout:
 *   [0] TerSafe_Init(initStruct*)       — ret 4, returns 0=ok/1=err/3=null
 *   [1] TerSafe_RecvData(dataBuffer*)   — ret 4, returns 0=ok
 *   [2] TerSafe_ProcessPacket(pktWrap*) — ret 4, returns 1=forward/0=consumed
 *   [3] TerSafe_AckCmd(cmdId)           — ret 4, returns 0=not handled/!0=handled
 *   [4] TerSafe_Tick()                  — ret 0, returns 0
 *   [5] TerSafe_Release()               — ret 0, returns 0
 * ============================================================ */

static void *g_co_vtable[32];
static char  g_co_obj[256] __attribute__((aligned(16)));

/*
 * co_vt[0]: thiscall, 1 arg — INIT
 *
 * From de_vt[0] analysis, initStruct is 16 bytes:
 *   +0x00 word:  type/version (must be non-zero)
 *   +0x04 dword: data pointer (must be non-zero)
 *   +0x08 dword: string pointer (optional)
 *   +0x0C dword: extra value
 *
 * Real impl: validates, allocates internal objects, sets global
 * "initialized" flag. All other methods check this flag.
 *
 * Our stub: copy the init data into the object, set the initialized
 * state so callers see us as properly initialized.
 */
static int g_tersafe_initialized = 0;

static int __fastcall co_vt_0(void *ecx, void *edx, void *a1) {
    g_call_count++;
    ts_log("[co_vt0] this=%p initStruct=%p ret=0x%X\n",
           ecx, a1, (uint32_t)__builtin_return_address(0));

    if (!a1) {
        ts_log("  NULL initStruct, returning 3\n");
        return 3;
    }

    uint32_t *d = (uint32_t *)a1;
    uint16_t type_val = *(uint16_t *)a1;
    ts_log("  type=0x%04X data_ptr=0x%08X str_ptr=0x%08X extra=0x%08X\n",
           type_val, d[1], d[2], d[3]);

    if (type_val == 0 || d[1] == 0) {
        ts_log("  invalid init data, returning 1\n");
        return 1;
    }

    /* Set object fields as observed from real TerSafe vt0:
     *   +0x14 = str_ptr from initStruct
     *   +0x18 = data_ptr from initStruct (0x0BED735E)
     *   +0x1C = UIN from initStruct (0x04D3)
     */
    uint8_t *obj = (uint8_t *)ecx;
    *(uint32_t *)(obj + 0x14) = d[2];       /* str_ptr */
    *(uint32_t *)(obj + 0x18) = d[1];       /* data_ptr */
    *(uint32_t *)(obj + 0x1C) = type_val;   /* UIN */
    g_tersafe_initialized = 1;

    ts_log("  set +0x14=0x%X +0x18=0x%X +0x1C=0x%X → returning 0\n",
           d[2], d[1], type_val);
    return 0;
}

/*
 * co_vt[1]: TerSafe_RecvData — thiscall, 1 arg (void* dataBuffer), ret 4
 *
 * Called from VM-protected wrapper (ret=0x123F58CA) with a heap pointer
 * to anti-cheat data received from the server (CSPkgTransAntiData).
 * Real impl: processes/decrypts the incoming anti-cheat challenge.
 *
 * Return: caller is VM-protected. Returning 1 to signal "processed ok".
 */
static int __fastcall co_vt_1(void *ecx, void *edx, void *dataBuffer) {
    g_call_count++;
    ts_log("[co_vt1:RecvData] this=%p buf=%p caller=0x%X\n",
           ecx, dataBuffer, (uint32_t)__builtin_return_address(0));

    /* Set "handshake complete" state as observed from real TerSafe:
     *   +0x5C = 0x7FFF (capability bits)
     *   +0xA0 = 0x00030007 (handshake done state — KEY field)
     *   +0xA4 = 0x00750000 (version)
     *   +0xA8-0xB8 = zeroed
     *   +0xBC = 0x73ED0B00 (hash)
     *   +0xD8 = 3 (state flag)
     *   +0xDC = 0x01460000
     */
    uint8_t *obj = (uint8_t *)ecx;
    *(uint32_t *)(obj + 0x5C) = 0x00007FFF;
    *(uint32_t *)(obj + 0xA0) = 0x00030007;
    *(uint32_t *)(obj + 0xA4) = 0x00750000;
    /* Clear +0xA8 to +0xB8 */
    memset(obj + 0xA8, 0, 0x14);
    *(uint32_t *)(obj + 0xBC) = 0x73ED0B00;
    *(uint32_t *)(obj + 0xD8) = 3;
    *(uint32_t *)(obj + 0xDC) = 0x01460000;
    /* Self-referencing pointers for +0xE0, +0xE4 */
    *(uint32_t *)(obj + 0xE0) = (uint32_t)(obj + 0xD8);
    *(uint32_t *)(obj + 0xE4) = (uint32_t)(obj + 0xC8);

    ts_log("  set handshake state (+0xA0=0x30007 +0xD8=3)\n");

    /*
     * Send CS_CMD_MULTI_NET_IPINFO (CMD 10) to trigger ListRoleRsp.
     *
     * co_vt[0] stored str_ptr at obj+0x14. str_ptr = &connection[+4].
     * connection = str_ptr - 4.
     * CryGame send function: FUN_10459870 at CryGame+0x459870
     * Calling convention: thiscall(connection, buffer_on_stack)
     * Buffer format: {data_ptr, seq=0, size} where data starts with CMD (BE uint16)
     *
     * CS_CMD_MULTI_NET_IPINFO = 10 (0x000A)
     * We send a minimal packet — server doesn't validate the Signature.
     */
    /*
     * str_ptr = &connection[+4], passed during init.
     * TerSafe uses this to send packets. Try reading *str_ptr as a send
     * interface and calling through the connection's vtable.
     *
     * connection = str_ptr - 4
     * connection[0] = vtable ptr
     * The send function FUN_10459870 is thiscall on the connection.
     * Rather than hardcoding CryGame offsets, read the vtable and try.
     *
     * Actually: str_ptr might directly contain a function pointer or
     * point to a send interface struct. Let's log what's there.
     */
    uint32_t str_ptr_val = *(uint32_t *)(obj + 0x14);
    if (str_ptr_val) {
        uint32_t val_at_str_ptr = *(uint32_t *)str_ptr_val;
        uint32_t connection = str_ptr_val - 4;
        uint32_t conn_vtable = *(uint32_t *)connection;
        ts_log("  str_ptr=0x%X *str_ptr=0x%X conn=0x%X conn_vtable=0x%X\n",
               str_ptr_val, val_at_str_ptr, connection, conn_vtable);
    }

    ts_log("  → returning 0\n");
    return 0;
}

/*
 * co_vt[2]: TerSafe_ProcessPacket — thiscall, 1 arg (void* pktWrapper), ret 4
 *
 * Called for EVERY network packet. pktWrapper layout:
 *   +0x00 uint32: internal CMD (0xA=init, 0x15=setup, 0xA0=config,
 *                                0x504=timer_tick, 0x45=disconnect, 0x7=misc)
 *   +0x04 void*:  send context pointer
 *   +0x08 uint32: data length
 *   +0x0C void*:  recv context pointer
 *   +0x10 uint32: connection ID
 *   +0x14 uint32: timestamp (unix epoch, seconds)
 *   +0x18 uint32: sequence/hash
 *   +0x1C void*:  pointer to actual packet data
 *
 * Return: 0 = packet consumed internally (anti-cheat only, don't forward)
 *         1 = packet passes through → triggers co_vt[3] call for forwarding
 *         (non-zero with bad value crashes — caller dereferences or branches)
 */
static int __fastcall co_vt_2(void *ecx, void *edx, void *pktWrapper) {
    g_call_count++;
    if (pktWrapper) {
        uint32_t *d = (uint32_t *)pktWrapper;
        ts_log("[co_vt2:ProcessPkt] cmd=0x%X d[1]=0x%X d[2]=0x%X d[3]=0x%X d[4]=0x%X d[5]=0x%X d[6]=0x%X d[7]=0x%X caller=0x%X\n",
               d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],
               (uint32_t)__builtin_return_address(0));
    }

    /* Set object state on first packet (cmd=0xA) as observed from real TerSafe:
     * +0x68 = 0x00040007
     * +0x6C = 0x00750000
     * +0x70 = code_ptr (real TerSafe sets a .text function ptr — packet delivery callback)
     * +0x74 = self-ref (heap ptr in real)
     * +0x84-0x90 = zeroed
     * +0x94 = timestamp
     * +0x98 = 0x114
     * +0xA0 = 0x0A (cmd)
     * +0xA4 = 0x01460000
     */
    if (pktWrapper) {
        uint32_t cmd = ((uint32_t *)pktWrapper)[0];
        uint8_t *obj = (uint8_t *)ecx;
        if (cmd == 0x0A && *(uint32_t *)(obj + 0x68) == 0) {
            *(uint32_t *)(obj + 0x68) = 0x00040007;
            *(uint32_t *)(obj + 0x6C) = 0x00750000;
            /* +0x70: packet delivery callback — point to our own pass-through func */
            *(uint32_t *)(obj + 0x70) = 0xDEADBEEF;  /* test: if called → crash. If not → not the issue */
            *(uint32_t *)(obj + 0x74) = (uint32_t)(obj + 0x80);  /* self-ref */
            *(uint32_t *)(obj + 0x78) = 0xE5003C00;  /* from dump */
            *(uint32_t *)(obj + 0x7C) = 0x871C792E;  /* from dump */
            *(uint32_t *)(obj + 0x80) = 0x00000007;  /* from dump */
            memset(obj + 0x84, 0, 0x10);
            *(uint32_t *)(obj + 0x98) = 0x00000100;  /* counter */
            *(uint32_t *)(obj + 0xA0) = 0x0A;
            *(uint32_t *)(obj + 0xA4) = 0x01460000;
            *(uint32_t *)(obj + 0xA8) = (uint32_t)(obj + 0x38);
            *(uint32_t *)(obj + 0xAC) = (uint32_t)(obj + 0x28);
            /* Also set from dump: */
            *(uint32_t *)(obj + 0x40) = 0xC7003000;
            *(uint32_t *)(obj + 0x44) = 0xA2000007;
            *(uint32_t *)(obj + 0x58) = 0x00001220;
            ts_log(" (set full state from dump)");
        }
    }
    /* Real TerSafe returns 1 for all packets. Dispatch happens INSIDE
     * the VM wrapper based on object state, not the return value. */
    ts_log(" → 1\n");
    return 1;
}

/*
 * co_vt[3]: TerSafe_QueryState — thiscall, 1 arg (uint32 queryParam), ret 4
 *
 * Called after co_vt[2] returns non-zero, and also independently.
 * CTerSafeProxy wraps this in QueryPerformanceCounter timing — result
 * feeds into CSessionTSSTimeNotify (TotalTime, MaxTime, PkgCount).
 *
 * Observed queryParam values: 0x14, 0x02, 0x16, 0xA1
 *
 * Wrapper does: test eax,eax / setne bl / return bl
 *   Return 0 = "not handled" → wrapper returns 0
 *   Return non-zero = "handled" → wrapper returns 1
 *   (Wrapper returns 2 if TerSafe not loaded at all)
 */
static int __fastcall co_vt_3(void *ecx, void *edx, uint32_t queryParam) {
    g_call_count++;
    ts_log("[co_vt3:QueryState] this=%p param=0x%X caller=0x%X → returning 0\n",
           ecx, queryParam, (uint32_t)__builtin_return_address(0));
    return 0;
}

/*
 * co_vt[4]: TerSafe_Tick — thiscall, 0 args, ret 0
 *
 * Periodic timer callback. In real TerSafe, handles keepalives
 * and scheduled anti-cheat checks. Not observed in current sessions
 * (only CMD 0x504 timer packets seen through co_vt[2]).
 *
 * Return: 0 = ok
 */
static int __fastcall co_vt_4(void *ecx, void *edx) {
    g_call_count++;
    ts_log("[co_vt4:Tick] this=%p caller=0x%X → returning 0\n",
           ecx, (uint32_t)__builtin_return_address(0));
    return 0;
}

/*
 * co_vt[5]: TerSafe_Release — thiscall, 0 args, ret 0
 *
 * Cleanup/shutdown. Called when CTerSafeProxy is destroyed.
 * Caller at 0x1045BAAD (CryGame.dll .text).
 *
 * Return: 0 = ok
 */
static int __fastcall co_vt_5(void *ecx, void *edx) {
    g_call_count++;
    ts_log("[co_vt5:Release] this=%p caller=0x%X → returning 0\n",
           ecx, (uint32_t)__builtin_return_address(0));
    return 0;
}

/*
 * Catch-all for unknown slots. Uses naked asm to avoid
 * any stack cleanup — logs slot + stack and returns 0.
 * The caller is responsible for stack cleanup (cdecl behavior).
 * If this causes stack corruption, we know a higher slot is
 * being called and we can determine its real cleanup from the log.
 */
static int __fastcall co_vt_catch(void *ecx, void *edx,
    int a1, int a2, int a3, int a4) {
    g_call_count++;
    ts_log("[co_vt_CATCH] this=%p a1=0x%X a2=0x%X a3=0x%X a4=0x%X ret=0x%X\n",
           ecx, a1, a2, a3, a4, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* Per-slot wrappers with logging so we know WHICH slot was called */
static int __fastcall co_vt_6(void *ecx, void *edx, int a1) {
    g_call_count++;
    ts_log("[co_vt6] this=%p a1=0x%X ret=0x%X\n",
           ecx, a1, (uint32_t)__builtin_return_address(0));
    return 0;
}
static int __fastcall co_vt_7(void *ecx, void *edx, int a1) {
    g_call_count++;
    ts_log("[co_vt7] this=%p a1=0x%X ret=0x%X\n",
           ecx, a1, (uint32_t)__builtin_return_address(0));
    return 0;
}
static int __fastcall co_vt_8(void *ecx, void *edx, int a1) {
    g_call_count++;
    ts_log("[co_vt8] this=%p a1=0x%X ret=0x%X\n",
           ecx, a1, (uint32_t)__builtin_return_address(0));
    return 0;
}
static int __fastcall co_vt_9(void *ecx, void *edx, int a1) {
    g_call_count++;
    ts_log("[co_vt9] this=%p a1=0x%X ret=0x%X\n",
           ecx, a1, (uint32_t)__builtin_return_address(0));
    return 0;
}
static int __fastcall co_vt_10(void *ecx, void *edx, int a1) {
    g_call_count++;
    ts_log("[co_vt10] this=%p a1=0x%X ret=0x%X\n",
           ecx, a1, (uint32_t)__builtin_return_address(0));
    return 0;
}

static void init_co_vtable(void) {
    g_co_vtable[0] = co_vt_0;
    g_co_vtable[1] = co_vt_1;
    g_co_vtable[2] = co_vt_2;
    g_co_vtable[3] = co_vt_3;
    g_co_vtable[4] = co_vt_4;
    g_co_vtable[5] = co_vt_5;
    g_co_vtable[6] = co_vt_6;
    g_co_vtable[7] = co_vt_7;
    g_co_vtable[8] = co_vt_8;
    g_co_vtable[9] = co_vt_9;
    g_co_vtable[10] = co_vt_10;
    for (int i = 11; i < 32; i++) g_co_vtable[i] = co_vt_catch;

    memset(g_co_obj, 0, sizeof(g_co_obj));
    *(void **)g_co_obj = g_co_vtable;

    /* Populate from real object dump (tersafe_monitor.log).
     * Self-referencing pointers replace heap ptrs so dereferences
     * land back in our object instead of crashing on NULL/garbage. */
    *(void **)(g_co_obj + 0x04) = g_co_obj;      /* was secondary vtable ptr */
    *(void **)(g_co_obj + 0x08) = g_co_obj;      /* was tertiary vtable ptr */
    *(void **)(g_co_obj + 0x0C) = g_co_obj + 0x80; /* was heap sub-object */
    *(void **)(g_co_obj + 0x10) = g_co_obj + 0x90; /* was heap sub-object */
    *(uint32_t *)(g_co_obj + 0x1C) = 0x4D3;      /* from real dump */
    /* offsets 0x20-0x27 = -1 (invalid handles) */
    memset(g_co_obj + 0x20, 0xFF, 8);
    *(void **)(g_co_obj + 0x38) = g_co_obj + 0xA0; /* was heap ptr */
    *(void **)(g_co_obj + 0x48) = g_co_obj + 0xB0; /* was heap ptr */
    *(uint32_t *)(g_co_obj + 0x4C) = 8;           /* from real dump */

    /* Make sub-object regions at +0x80..+0xBF also self-referencing */
    for (int i = 0x80; i < 0xC0; i += 4)
        *(void **)(g_co_obj + i) = g_co_obj;
}


/* ============================================================
 * Exports
 * ============================================================ */

__declspec(dllexport) void * __cdecl CreateObj(void) {
    ts_log("CreateObj -> %p\n", g_co_obj);
    return g_co_obj;
}

/* DllEntry: returns 8-byte object with version 0x400 */
__declspec(dllexport) void * __cdecl DllEntry(void) {
    ts_log("DllEntry -> %p\n", g_de_obj);
    return g_de_obj;
}

/* DllEntry1: stdcall, 1 arg. Init — stores internal object globally. */
static void *g_main_obj = NULL;

__declspec(dllexport) int __stdcall DllEntry1(void *initStruct) {
    ts_log("DllEntry1 init=%p\n", initStruct);
    g_main_obj = g_int_obj;
    /* Call int_vt[0](initStruct) like the real code does */
    if (initStruct) {
        typedef int (__fastcall *VT0)(void*, void*, void*);
        ((VT0)g_int_vtable[0])(g_int_obj, NULL, initStruct);
    }
    return 0;
}

/* DllEntry2-5: VM-protected in original. Stub as cdecl returning 0.
 * If these crash, check caller for actual convention. */
__declspec(dllexport) int __cdecl DllEntry2(void) {
    ts_log("DllEntry2\n");
    return 0;
}

__declspec(dllexport) int __cdecl DllEntry3(void) {
    ts_log("DllEntry3\n");
    return 0;
}

__declspec(dllexport) int __cdecl DllEntry4(void) {
    ts_log("DllEntry4\n");
    return 0;
}

__declspec(dllexport) int __cdecl DllEntry5(void) {
    ts_log("DllEntry5\n");
    return 0;
}

/* DllEntry6: cdecl, 0 args. Calls internal obj->vt[1](). */
__declspec(dllexport) int __cdecl DllEntry6(void) {
    ts_log("DllEntry6\n");
    if (g_main_obj) {
        typedef int (__fastcall *VT1)(void*, void*);
        ((VT1)g_int_vtable[1])(g_int_obj, NULL);
    }
    return 0;
}

/* DllEntry7: cdecl, 0 args. NOP. */
__declspec(dllexport) int __cdecl DllEntry7(void) {
    return 0;
}

/* DllEntry8: cdecl, 0 args. Returns internal object. */
__declspec(dllexport) void * __cdecl DllEntry8(void) {
    ts_log("DllEntry8 -> %p\n", g_int_obj);
    return g_int_obj;
}


/* ============================================================
 * DllMain
 * ============================================================ */

/* Raw write — works even if CRT isn't initialized yet */
static void raw_log(const char *msg) {
    HANDLE hf = CreateFileA("tersafe_stub.log",
        FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf != INVALID_HANDLE_VALUE) {
        DWORD wrote;
        WriteFile(hf, msg, strlen(msg), &wrote, NULL);
        CloseHandle(hf);
    }
    OutputDebugStringA(msg);
}

BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(h);

        /* Immediate indicator using raw Win32 — no CRT dependency */
        raw_log("TerSafe stub v2: DllMain ATTACH\n");

        char path[MAX_PATH];
        GetModuleFileNameA(h, path, MAX_PATH);  /* h = our DLL, not NULL */
        raw_log("  DLL path: ");
        raw_log(path);
        raw_log("\n");

        /* Open log for detailed logging */
        char *s = strrchr(path, '\\');
        if (s) *(s + 1) = '\0';
        strcat(path, "tersafe_stub.log");
        g_log = fopen(path, "w");  /* overwrite each run */

        ts_log("TerSafe stub v2 (signature-correct)\n");
        ts_log("  co_obj=%p de_obj=%p int_obj=%p\n",
               g_co_obj, g_de_obj, g_int_obj);

        init_co_vtable();
        init_de_vtable();
        init_int_vtable();

        ts_log("Ready\n");
    }
    if (reason == DLL_PROCESS_DETACH) {
        ts_log("Unload (calls: %d)\n", g_call_count);
        if (g_log) fclose(g_log);
        raw_log("TerSafe stub: DllMain DETACH\n");
    }
    return TRUE;
}
