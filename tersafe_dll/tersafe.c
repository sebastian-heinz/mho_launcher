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

/* ============================================================
 * Logging
 * ============================================================ */

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
 * Send interface — calls through tpdu_conn vtable
 *
 * The stub receives proxy_ptr (CTerSafeProxy+4) during co_vt[0] init.
 * proxy_ptr + 8 = CTerSafeProxy+0xC = tpdu_conn (TPDU connection object).
 * tpdu_conn has a C++ vtable with send/recv methods.
 *
 * tpdu_conn vtable (12 entries captured at runtime):
 *   [0]-[11] — methods in the TPDU networking library
 *   One of these is the send method (TBD — needs testing)
 * ============================================================ */

static uint32_t g_tpdu_conn = 0;  /* cached from proxy_ptr + 8 */

/* ============================================================
 * Shared struct definitions
 * ============================================================ */

/* Init struct passed to co_vt[0] by the VM wrapper (16 bytes) */
typedef struct {
    uint16_t uin;           /* +0x00: game UIN / type (must be non-zero) */
    uint16_t _pad0;
    uint32_t data_ptr;      /* +0x04: opaque data value (must be non-zero) */
    uint32_t proxy_ptr;     /* +0x08: pointer to CTerSafeProxy+4 (str_ptr) */
    uint32_t extra;         /* +0x0C: extra value (stack address) */
} TsInitStruct;

/* Packet wrapper passed to co_vt[2] for every network packet (32 bytes) */
typedef struct {
    uint32_t cmd;           /* +0x00: internal CMD ID (0xA, 0x15, 0xA0, 0x504, ...) */
    uint32_t proxy_ac;      /* +0x04: CTerSafeProxy + 0xAC (constant per session) */
    uint32_t data_len;      /* +0x08: packet data length */
    uint32_t field_0c;      /* +0x0C: constant per session (heap object) */
    uint32_t conn_handle;   /* +0x10: connection handle (constant per session) */
    uint32_t timestamp;     /* +0x14: unix epoch seconds (incrementing) */
    uint32_t sequence;      /* +0x18: packet sequence number */
    uint32_t data_ptr;      /* +0x1C: pointer to raw packet data */
} TsPacketWrapper;

/* CreateObj object layout (256 bytes)
 *
 * The VM wrapper reads/writes fields in this object to track TerSafe state.
 * Fields marked "dump" were captured from a running real TerSafe session.
 * Fields marked "self-ref" are set to point back into this object so that
 * pointer dereferences don't crash on NULL/garbage.
 */
typedef struct {
    void    *vtable;            /* +0x00: CreateObj vtable pointer */
    void    *self_04;           /* +0x04: self-ref (secondary vtable in real) */
    void    *self_08;           /* +0x08: self-ref (tertiary vtable in real) */
    void    *sub_obj_0c;        /* +0x0C: self-ref → +0x80 (heap sub-object in real) */
    void    *sub_obj_10;        /* +0x10: self-ref → +0x90 (heap sub-object in real) */
    uint32_t proxy_ptr;         /* +0x14: str_ptr from init (CTerSafeProxy+4) */
    uint32_t data_ptr;          /* +0x18: data_ptr from init */
    uint32_t uin;               /* +0x1C: UIN from init (0x4D3) */
    uint32_t handles[2];        /* +0x20: -1 (invalid handles) */
    uint8_t  _pad_28[0x10];     /* +0x28..+0x37 */
    void    *heap_ptr_38;       /* +0x38: self-ref → +0xA0 */
    uint8_t  _pad_3c[0x04];     /* +0x3C..+0x3F */
    uint32_t dump_40;           /* +0x40: 0xC7003000 (from dump) */
    uint32_t dump_44;           /* +0x44: 0xA2000007 (from dump) */
    void    *heap_ptr_48;       /* +0x48: self-ref → +0xB0 */
    uint32_t dump_4c;           /* +0x4C: 8 (from dump) */
    uint8_t  _pad_50[0x08];     /* +0x50..+0x57 */
    uint32_t dump_58;           /* +0x58: 0x1220 (from dump) */
    uint32_t capability_bits;   /* +0x5C: 0x7FFF after handshake */
    uint8_t  _pad_60[0x08];     /* +0x60..+0x67 */
    uint32_t pkt_state;         /* +0x68: set to 0x00040007 on first CMD 0xA */
    uint32_t pkt_version;       /* +0x6C: 0x00750000 */
    uint32_t callback_ptr;      /* +0x70: packet delivery callback (0xDEADBEEF = unused) */
    void    *self_ref_74;       /* +0x74: self-ref → +0x80 */
    uint32_t crypto_78;         /* +0x78: 0xE5003C00 (from dump) */
    uint32_t crypto_7c;         /* +0x7C: 0x871C792E (from dump) */
    uint32_t sub_region[16];    /* +0x80..+0xBF: self-referencing sub-object region */
    uint8_t  _pad_c0[0x08];     /* +0xC0..+0xC7 */
    uint8_t  _ref_c8[0x10];     /* +0xC8..+0xD7: referenced by self-ref at +0xE4 */
    uint32_t handshake_state;   /* +0xD8: 3 after handshake */
    uint32_t handshake_ver;     /* +0xDC: 0x01460000 */
    uint32_t self_ref_e0;       /* +0xE0: points to +0xD8 */
    uint32_t self_ref_e4;       /* +0xE4: points to +0xC8 */
    uint8_t  _pad_e8[0x18];     /* +0xE8..+0xFF */
    /* Total: 256 bytes */
} TsCreateObj;

_Static_assert(sizeof(TsCreateObj) == 256, "TsCreateObj must be 256 bytes");

/* The real TerSafe also exports CreateObj's siblings: `DllEntry` (returns a
 * 16-byte version/interface object) and `DllEntry8` (returns a 0x260-byte
 * internal processor object). MHO/CryGame never calls any of them, so our
 * stub returns NULL and only logs if the game ever starts exercising them.
 * No object layouts or vtables are needed. */

/* Handshake state constants */
#define TS_HANDSHAKE_DONE       0x00030007
#define TS_HANDSHAKE_VERSION    0x00750000
#define TS_HANDSHAKE_HASH       0x73ED0B00
#define TS_HANDSHAKE_FLAG       3
#define TS_HANDSHAKE_DC         0x01460000
#define TS_CAPABILITY_FULL      0x00007FFF

/* Packet state constants (set on first CMD 0xA) */
#define TS_PKT_STATE_INIT       0x00040007
#define TS_PKT_VERSION          0x00750000

/* ============================================================
 * __thiscall emulation
 *
 * Original TerSafe vtable methods are C++ __thiscall:
 *   - ECX = this pointer (object whose method is being called)
 *   - remaining args pushed right-to-left on the stack
 *   - callee cleans up the stack (ret N)
 *
 * mingw-w64 has no __thiscall keyword for free functions, so we emulate it
 * with __fastcall:
 *
 *     __fastcall f(<ECX>, <EDX>, <stack args...>)
 *
 * __fastcall normally puts the first two integer args in ECX and EDX. In
 * __thiscall only ECX is used for `this`; EDX is NOT an input. We still
 * have to declare an EDX parameter though, otherwise __fastcall would put
 * our first "real" stack arg into EDX and silently misaligned everything.
 * The THISCALL_EDX placeholder consumes the register slot and documents
 * that the value is uninitialised — do not read it.
 *
 * Who supplies ECX?  The caller. When CryGame calls
 *     pObj->some_virtual_method(arg)
 * the compiler emits
 *     mov  ecx, pObj
 *     push arg
 *     call [ecx+slot*4]          ; indirect through pObj's vtable
 * so on entry to our function, ECX holds the exact pointer CryGame held.
 * For the TerSafe main vtable that pointer is what we returned from
 * CreateObj() — &g_tersafe_obj, a TsCreateObj* — hence the first parameter
 * is typed `TsCreateObj *self`.
 * ============================================================ */
#define THISCALL_EDX void *_edx  /* reserved — __thiscall doesn't use EDX */


/* ============================================================
 * CreateObj Object — TerSafe main interface
 *
 * This is the one and only TerSafe interface MHO actually uses. 6 methods,
 * all VM-protected (.tvm0) in the original binary. Signatures deduced from
 * caller analysis + runtime probing:
 *
 *   [0] TerSafe_Init(TsInitStruct *)    — ret 4, returns 0=ok / 1=err / 3=null
 *   [1] TerSafe_RecvData(void *buf)     — ret 4, returns 0=ok
 *   [2] TerSafe_ProcessPacket(pkt *)    — ret 4, returns 1=forward / 0=consumed
 *   [3] TerSafe_QueryState(u32 code)    — ret 4, returns 0=not-handled / !0=handled
 *   [4] TerSafe_Tick()                  — ret 0
 *   [5] TerSafe_Release()               — ret 0
 * ============================================================ */

static void *g_tersafe_vtable[32];
static TsCreateObj g_tersafe_obj __attribute__((aligned(16)));
static int g_tersafe_initialized = 0;

/* [0] TerSafe_Init — __thiscall, 1 stack arg, ret 4.
 *
 * CryGame calls this once immediately after CreateObj() to hand us the
 * 16-byte init struct.  `init->proxy_ptr` is the key field — it's the
 * address of CTerSafeProxy+4 inside CryGame, which is how we later reach
 * the TPDU connection object (proxy_ptr + 8 → tpdu_conn).
 *
 * Return codes are defined by observation:
 *   0 = success (state cached on self)
 *   1 = invalid init data (zero uin or zero data_ptr)
 *   3 = null init pointer
 */
static int __fastcall tersafe_vt_0(
    TsCreateObj  *self,    /* ECX — our main object (= &g_tersafe_obj) */
    THISCALL_EDX,
    TsInitStruct *init)    /* ESP+4 — {uin, data_ptr, proxy_ptr, extra} */
{
    g_call_count++;
    ts_log("[tersafe_vt0:Init] this=%p init=%p caller=0x%X\n",
           self, init, (uint32_t)__builtin_return_address(0));

    if (!init) {
        ts_log("  NULL init struct → returning 3\n");
        return 3;
    }

    ts_log("  uin=0x%04X data_ptr=0x%08X proxy_ptr=0x%08X extra=0x%08X\n",
           init->uin, init->data_ptr, init->proxy_ptr, init->extra);

    if (init->uin == 0 || init->data_ptr == 0) {
        ts_log("  invalid init data → returning 1\n");
        return 1;
    }

    self->proxy_ptr = init->proxy_ptr;
    self->data_ptr  = init->data_ptr;
    self->uin       = init->uin;
    g_tersafe_initialized = 1;

    ts_log("  cached proxy_ptr=0x%X data_ptr=0x%X uin=0x%X → returning 0\n",
           self->proxy_ptr, self->data_ptr, self->uin);
    return 0;
}

/* [1] TerSafe_RecvData — __thiscall, 1 stack arg, ret 4.
 *
 * Called with a heap pointer to anti-cheat response data the server sent
 * inside CSPkgTransAntiData (CMD 1285). The real TerSafe VM parses and
 * reacts. Our stub flips the observed "handshake complete" bits on self
 * and caches the TPDU connection pointer for later use.
 */
static int __fastcall tersafe_vt_1(
    TsCreateObj *self,             /* ECX */
    THISCALL_EDX,
    void        *anti_cheat_buf)   /* ESP+4 — heap buffer, length encoded inside */
{
    g_call_count++;
    ts_log("[tersafe_vt1:RecvData] this=%p buf=%p caller=0x%X\n",
           self, anti_cheat_buf, (uint32_t)__builtin_return_address(0));

    /* Set "handshake complete" state as observed from real TerSafe */
    self->capability_bits = TS_CAPABILITY_FULL;
    self->handshake_state = TS_HANDSHAKE_FLAG;
    self->handshake_ver   = TS_HANDSHAKE_DC;
    self->self_ref_e0     = (uint32_t)&self->handshake_state;
    self->self_ref_e4     = (uint32_t)&self->_ref_c8;

    ts_log("  set handshake state (capability=0x%X state=%u)\n",
           self->capability_bits, self->handshake_state);

    /* Cache tpdu_conn from CTerSafeProxy+0xC = proxy_ptr + 8 */
    if (self->proxy_ptr) {
        g_tpdu_conn = *(uint32_t *)(self->proxy_ptr + 8);
        ts_log("  proxy_ptr=0x%X tpdu_conn=0x%X\n", self->proxy_ptr, g_tpdu_conn);
        if (g_tpdu_conn > 0x10000) {
            uint32_t *vt = *(uint32_t **)g_tpdu_conn;
            /* vtable has 30 slots. Slot [16] is Send (proto+0x36F30). */
            ts_log("  tpdu_vt=%p slots:", vt);
            for (int i = 0; i < 30; i++) ts_log(" [%d]=0x%X", i, vt[i]);
            ts_log("\n");

            /* CMD 10 (CS_CMD_MULTI_NET_IPINFO) send probe.
             *
             * The tpdu_conn 30-slot vtable is for TPDU control-plane; slot [16]
             * hardcodes TpduCmd=0x0B (MBA_VERIFYREQ) via FUN_10035140 inside
             * FUN_10036F30, so it's the wrong door for CS packets (which need
             * TpduCmd=NONE=0). Instead: build the full plaintext frame ourselves
             * and call protocalhandler!FUN_10043020 (TqqapiSend) directly on the
             * socket sub-object at tpdu_conn+0x1B4. Server's _tdpuCrypto is null
             * so no encrypt needed.
             *
             * Wire layout:
             *   [1]  0x55  Magic
             *   [1]  0x0E  Version
             *   [1]  0x00  TpduCmd = NONE
             *   [1]  0x00  EncHeadLen
             *   [4]  BE32  HeadLen  = 12
             *   [4]  BE32  BodyLen  = 16 (CS head) + 165 (CSMultiNetIpinfo body)
             *   [2]  BE16  CS Cmd   = 0x000A
             *   [2]  BE16  CS HeadLen = 16
             *   [4]  BE32  CS BodyLen = 165
             *   [4]  BE32  CS SeqId  = 1
             *   [4]  BE32  CS NoUse  = 0
             *   [165 bytes] TDR-encoded CSMultiNetIpinfo (all zeros / empty
             *   strings / zero-length signature)
             */
            static int probed = 0;
            if (!probed) {
                probed = 1;

                uint32_t runtime_base = vt[16] - 0x123F;
                void *socket_obj = *(void **)(g_tpdu_conn + 0x1B4);

                typedef int (__fastcall *tqqapi_send_t)(
                    void *ecx_, void *edx_,
                    uint8_t *frame, int p2, uint8_t *p3, int p4);
                tqqapi_send_t tqqapi_send =
                    (tqqapi_send_t)(runtime_base + 0x43020);

                /* Build CSMultiNetIpinfo body (165 bytes). */
                enum {
                    BODY_LEN = 15 + 4 + 50 + 40 + 4 + 4 + 4 + 4 + 40, /* = 165 */
                    TPDU_BASE_HEAD = 12,
                    CS_HEAD = 16,
                    FRAME_LEN = TPDU_BASE_HEAD + CS_HEAD + BODY_LEN,
                };
                static uint8_t frame[FRAME_LEN];
                memset(frame, 0, FRAME_LEN);

                uint8_t *p = frame;
                /* TPDU base header (12 bytes) */
                p[0] = 0x55; p[1] = 0x0E; p[2] = 0x00; p[3] = 0x00;
                /* HeadLen = 12 (big-endian) */
                p[4] = 0; p[5] = 0; p[6] = 0; p[7] = 0x0C;
                /* BodyLen = CS_HEAD + BODY_LEN = 181 (big-endian) */
                uint32_t tpdu_body = CS_HEAD + BODY_LEN;
                p[8]  = (uint8_t)(tpdu_body >> 24);
                p[9]  = (uint8_t)(tpdu_body >> 16);
                p[10] = (uint8_t)(tpdu_body >> 8);
                p[11] = (uint8_t)(tpdu_body);

                /* CS header (16 bytes) at offset 12 */
                p[12] = 0x00; p[13] = 0x0A;           /* CS Cmd = 10 */
                p[14] = 0x00; p[15] = 0x10;           /* CS HeadLen = 16 */
                p[16] = 0; p[17] = 0;
                p[18] = (uint8_t)(BODY_LEN >> 8);
                p[19] = (uint8_t)(BODY_LEN);          /* CS BodyLen = 165 */
                p[20] = 0; p[21] = 0; p[22] = 0; p[23] = 1;  /* SeqId = 1 */
                /* p[24..27] = NoUse = 0 (already zeroed) */

                /* CS body at offset 28: TDR-encoded CSMultiNetIpinfo.
                 * Empty strings are "int32 BE len=1, then '\0'".
                 * Zero ints / zero-length list encode as zeros. memset above
                 * already zeroed; just patch the three leading string lengths
                 * and the 10 ConfigIp lengths to 1. */
                uint8_t *b = p + TPDU_BASE_HEAD + CS_HEAD;
                /* SelectIp len = 1 */
                b[0]=0; b[1]=0; b[2]=0; b[3]=1;  /* b[4]=0 null byte */
                /* DomainName len = 1 */
                b[5]=0; b[6]=0; b[7]=0; b[8]=1;
                /* DomainAnalyseIp len = 1 */
                b[10]=0; b[11]=0; b[12]=0; b[13]=1;
                /* PingDomainIp = 0 at b[15..18] (already zero) */
                /* ConfigIp[10]: 10 empty strings at b[19..] */
                for (int i = 0; i < 10; i++) {
                    uint8_t *s = b + 19 + i * 5;
                    s[0]=0; s[1]=0; s[2]=0; s[3]=1;   /* s[4]=0 null */
                }
                /* PingIp[10] at b[69..108] (zeros, already zero) */
                /* Port = 0 at b[109..112] */
                /* Signature list: count=0 at b[113..116] */
                /* Isp=0 at b[117..120] */
                /* Mode=0 at b[121..124] */
                /* HistoryPingWeight[10]=0 at b[125..164] */

                ts_log("  [cmd10] socket=%p tqqapi_send=%p frame=%p (%d bytes)\n",
                       socket_obj, tqqapi_send, frame, FRAME_LEN);

                int rc = tqqapi_send(socket_obj, 0, frame, 0, NULL, 100);

                ts_log("  [cmd10] rc=%d\n", rc);
            }
        }
    }

    ts_log("  → returning 0\n");
    return 0;
}

/* [2] TerSafe_ProcessPacket — __thiscall, 1 stack arg, ret 4.
 *
 * CryGame calls this for every inbound network packet, BEFORE dispatching
 * the packet to game code.  `pkt` is a 32-byte wrapper carrying the CS
 * command id, length, sequence, session-constant handles, and a pointer to
 * the raw packet bytes. Return 1 to let the game process it, 0 to consume.
 */
static int __fastcall tersafe_vt_2(
    TsCreateObj     *self,   /* ECX */
    THISCALL_EDX,
    TsPacketWrapper *pkt)    /* ESP+4 — 32-byte wrapper (may be NULL defensively) */
{
    g_call_count++;

    if (pkt) {
        ts_log("[tersafe_vt2:ProcessPkt] cmd=0x%X proxy_ac=0x%X len=0x%X "
               "f0c=0x%X conn=0x%X ts=0x%X seq=0x%X data=0x%X caller=0x%X\n",
               pkt->cmd, pkt->proxy_ac, pkt->data_len, pkt->field_0c,
               pkt->conn_handle, pkt->timestamp, pkt->sequence, pkt->data_ptr,
               (uint32_t)__builtin_return_address(0));

        /* Set object state on first packet (cmd=0xA) */
        if (pkt->cmd == 0x0A && self->pkt_state == 0) {
            self->pkt_state    = TS_PKT_STATE_INIT;
            self->pkt_version  = TS_PKT_VERSION;
            self->callback_ptr = 0xDEADBEEF;    /* unused — crash if invoked */
            self->self_ref_74  = (void *)&self->sub_region[0];
            self->crypto_78    = 0xE5003C00;
            self->crypto_7c    = 0x871C792E;
            self->sub_region[0] = 0x00000007;
            memset(&self->sub_region[1], 0, 0x10);
            self->dump_40 = 0xC7003000;
            self->dump_44 = 0xA2000007;
            self->dump_58 = 0x00001220;
            ts_log(" (set initial state)");
        }
    }

    ts_log(" → 1\n");
    return 1;
}

/* [3] TerSafe_QueryState — __thiscall, 1 stack arg, ret 4.
 *
 * CryGame asks "do you know / want to handle event X?". Observed codes:
 * 0x02, 0x14, 0x16, 0xA1. We always answer 0 = not-handled so the game
 * falls back to its own behaviour.
 */
static int __fastcall tersafe_vt_3(
    TsCreateObj *self,       /* ECX */
    THISCALL_EDX,
    uint32_t     query_code) /* ESP+4 */
{
    g_call_count++;
    ts_log("[tersafe_vt3:QueryState] this=%p query_code=0x%X caller=0x%X → 0\n",
           self, query_code, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* [4] TerSafe_Tick — __thiscall, 0 args, ret 0. Timer / heartbeat hook. */
static int __fastcall tersafe_vt_4(TsCreateObj *self, THISCALL_EDX) {
    g_call_count++;
    ts_log("[tersafe_vt4:Tick] this=%p caller=0x%X → 0\n",
           self, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* [5] TerSafe_Release — __thiscall, 0 args, ret 0. Called at shutdown. */
static int __fastcall tersafe_vt_5(TsCreateObj *self, THISCALL_EDX) {
    g_call_count++;
    ts_log("[tersafe_vt5:Release] this=%p caller=0x%X → 0\n",
           self, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* Catch-all for slot indices beyond the 6 the real TerSafe vtable has.
 * CryGame shouldn't touch these but we install this in the trailing slots
 * so a stray call logs instead of reading past the end of our vtable. */
static int __fastcall tersafe_vt_catch(
    TsCreateObj *self, THISCALL_EDX,
    int a1, int a2, int a3, int a4)
{
    g_call_count++;
    ts_log("[tersafe_vt_CATCH] this=%p a1=0x%X a2=0x%X a3=0x%X a4=0x%X ret=0x%X\n",
           self, a1, a2, a3, a4, (uint32_t)__builtin_return_address(0));
    return 0;
}

/* Per-slot wrappers for logging which slot was called.  These fill slots
 * [6..10] which the real vtable doesn't define — kept defensively. */
#define TERSAFE_VT_SLOT(N) \
static int __fastcall tersafe_vt_##N( \
    TsCreateObj *self, THISCALL_EDX, int arg) { \
    g_call_count++; \
    ts_log("[tersafe_vt" #N "] this=%p arg=0x%X ret=0x%X\n", \
           self, arg, (uint32_t)__builtin_return_address(0)); \
    return 0; \
}
TERSAFE_VT_SLOT(6) TERSAFE_VT_SLOT(7) TERSAFE_VT_SLOT(8) TERSAFE_VT_SLOT(9) TERSAFE_VT_SLOT(10)

static void init_tersafe_vtable(void) {
    g_tersafe_vtable[0] = tersafe_vt_0;
    g_tersafe_vtable[1] = tersafe_vt_1;
    g_tersafe_vtable[2] = tersafe_vt_2;
    g_tersafe_vtable[3] = tersafe_vt_3;
    g_tersafe_vtable[4] = tersafe_vt_4;
    g_tersafe_vtable[5] = tersafe_vt_5;
    g_tersafe_vtable[6] = tersafe_vt_6;
    g_tersafe_vtable[7] = tersafe_vt_7;
    g_tersafe_vtable[8] = tersafe_vt_8;
    g_tersafe_vtable[9] = tersafe_vt_9;
    g_tersafe_vtable[10] = tersafe_vt_10;
    for (int i = 11; i < 32; i++) g_tersafe_vtable[i] = tersafe_vt_catch;

    memset(&g_tersafe_obj, 0, sizeof(g_tersafe_obj));
    g_tersafe_obj.vtable = g_tersafe_vtable;

    /* Self-referencing pointers so dereferences land in our object */
    g_tersafe_obj.self_04 = &g_tersafe_obj;
    g_tersafe_obj.self_08 = &g_tersafe_obj;
    g_tersafe_obj.sub_obj_0c = (char *)&g_tersafe_obj + 0x80;
    g_tersafe_obj.sub_obj_10 = (char *)&g_tersafe_obj + 0x90;
    g_tersafe_obj.uin = 0x4D3;
    g_tersafe_obj.handles[0] = 0xFFFFFFFF;
    g_tersafe_obj.handles[1] = 0xFFFFFFFF;
    g_tersafe_obj.heap_ptr_38 = (char *)&g_tersafe_obj + 0xA0;
    g_tersafe_obj.heap_ptr_48 = (char *)&g_tersafe_obj + 0xB0;
    g_tersafe_obj.dump_4c = 8;

    /* Self-referencing sub-object region at +0x80..+0xBF */
    for (int i = 0; i < 16; i++)
        g_tersafe_obj.sub_region[i] = (uint32_t)&g_tersafe_obj;
}


/* ============================================================
 * Exports — ordered to match tersafe.def's ordinal sequence:
 *
 *   CreateObj   @1   <-- only one MHO actually calls
 *   DllEntry1   @2
 *   DllEntry2   @3
 *   DllEntry3   @4
 *   DllEntry4   @5
 *   DllEntry5   @6
 *   DllEntry6   @7
 *   DllEntry7   @8
 *   DllEntry8   @9
 *   DllEntry    @10
 *
 * DllEntry..DllEntry8 are all log-and-return-zero stubs. They must stay
 * exported so the PE loader can resolve MHO's import table, but we've
 * never observed any of them being called. If the log ever shows one
 * firing, we need to put real behaviour behind it — see
 * docs/tersafe_analysis.md for what the original TerSafe does.
 * ============================================================ */

/* @1 */
__declspec(dllexport) void * __cdecl CreateObj(void) {
    ts_log("CreateObj -> %p\n", &g_tersafe_obj);
    return &g_tersafe_obj;
}

/* @2 */
__declspec(dllexport) int __stdcall DllEntry1(void *initStruct) {
    ts_log("!! DllEntry1 called (unexpected) init=%p -> 0\n", initStruct);
    return 0;
}

/* @3 */
__declspec(dllexport) int __cdecl DllEntry2(void) {
    ts_log("!! DllEntry2 called (unexpected) -> 0\n"); return 0;
}

/* @4 */
__declspec(dllexport) int __cdecl DllEntry3(void) {
    ts_log("!! DllEntry3 called (unexpected) -> 0\n"); return 0;
}

/* @5 */
__declspec(dllexport) int __cdecl DllEntry4(void) {
    ts_log("!! DllEntry4 called (unexpected) -> 0\n"); return 0;
}

/* @6 */
__declspec(dllexport) int __cdecl DllEntry5(void) {
    ts_log("!! DllEntry5 called (unexpected) -> 0\n"); return 0;
}

/* @7 */
__declspec(dllexport) int __cdecl DllEntry6(void) {
    ts_log("!! DllEntry6 called (unexpected) -> 0\n"); return 0;
}

/* @8 */
__declspec(dllexport) int __cdecl DllEntry7(void) {
    ts_log("!! DllEntry7 called (unexpected) -> 0\n"); return 0;
}

/* @9 */
__declspec(dllexport) void * __cdecl DllEntry8(void) {
    ts_log("!! DllEntry8 called (unexpected) -> NULL\n"); return NULL;
}

/* @10 */
__declspec(dllexport) void * __cdecl DllEntry(void) {
    ts_log("!! DllEntry called (unexpected) -> NULL\n");
    return NULL;
}


/* ============================================================
 * DllMain
 * ============================================================ */

BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, LPVOID reserved) {
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(h);

        /* Resolve the DLL's on-disk path so we can drop tersafe_stub.log
         * next to the DLL (rather than in the game's CWD). Open g_log first
         * thing so every subsequent event is captured via ts_log. */
        char dll_path[MAX_PATH];
        GetModuleFileNameA(h, dll_path, MAX_PATH);

        char log_path[MAX_PATH];
        strncpy(log_path, dll_path, MAX_PATH - 1);
        log_path[MAX_PATH - 1] = '\0';
        char *sep = strrchr(log_path, '\\');
        if (sep) *(sep + 1) = '\0';
        strncat(log_path, "tersafe_stub.log",
                MAX_PATH - strlen(log_path) - 1);
        g_log = fopen(log_path, "w");

        ts_log("TerSafe stub v3: DllMain ATTACH\n");
        ts_log("  DLL path: %s\n", dll_path);
        ts_log("  tersafe_obj=%p\n", &g_tersafe_obj);

        init_tersafe_vtable();

        ts_log("Ready\n");
    }
    if (reason == DLL_PROCESS_DETACH) {
        ts_log("TerSafe stub: DllMain DETACH (calls=%d)\n", g_call_count);
        if (g_log) { fclose(g_log); g_log = NULL; }
    }
    return TRUE;
}
