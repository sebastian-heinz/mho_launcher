/*
 * TerSafe.dll stub — baseline-matched signatures, no state logic.
 *
 * Every signature, return value and pre-Init struct layout in this file was
 * verified against a real TerSafe session capture (ag_mho_log_baseline_*).
 * The stub intentionally contains NO anti-cheat logic or state transitions —
 * each vtable method logs its arguments and returns the constant that the
 * baseline shows real TerSafe returning, nothing more.  vt_0 (Init) is the
 * single exception: it copies three fields out of its input struct into
 * `this`, exactly matching the real function's only observed side effect.
 *
 * Slot summary (derived from baseline log):
 *
 *   [0] Init          1 stack arg   returns int=0       writes self+{0x14,0x18,0x1C}
 *   [1] Notify        0 stack args  returns int=0       log only (no observed writes)
 *   [2] ProcessPacket 1 stack arg   returns int=1       log only
 *   [3] QueryState    1 stack arg   returns int=0       log only
 *   [4] Tick          unobserved    returns int=0       log only
 *   [5] Release       0 stack args  returns int=0       log only
 *
 * The pre-Init `this` (returned by CreateObj before any vt method runs) has
 * the exact constants and heap-pointer-shaped self-refs that real TerSafe
 * populates in its allocator; see init_pre_state() below.
 */

#include <windows.h>
#include <string.h>
#include <stdint.h>

/* ============================================================
 * __thiscall emulation via __fastcall + THISCALL_EDX
 *
 * Real TerSafe vtable methods are MSVC __thiscall (ECX=this, stack args
 * right-to-left, callee cleanup via `ret N`).  mingw-w64 has no __thiscall
 * keyword for free functions, so we emulate with __fastcall: ECX → `self`,
 * EDX → `THISCALL_EDX` placeholder (caller-scratch, unused), then stack args
 * land at MSVC-__thiscall offsets.
 * ============================================================ */
#define THISCALL_EDX void *_edx_unused

/* ============================================================
 * Known data structures
 *
 * Any field marked (baseline) is verified from the ag_mho observation log.
 * Fields marked (unknown) we haven't observed a reliable meaning for and
 * leave as opaque dwords — we still encode their *pre-Init values* so the
 * object shape matches real TerSafe byte-for-byte at CreateObj time.
 * ============================================================ */

/*
 * TsInitStruct — 16+ byte argument to vt_0 Init.  Baseline shows vt_0 copies
 * exactly three fields from this struct into `self`:
 *     init->uin       → self->uin       (self+0x1C)
 *     init->data_ptr  → self->data_ptr  (self+0x18)
 *     init->proxy_ptr → self->proxy_ptr (self+0x14)
 */
typedef struct TsInitStruct {
    uint32_t uin;            /* +0x00  (baseline) game UIN; observed 0x000004D3 */
    uint32_t data_ptr;       /* +0x04  (baseline) opaque key; observed 0x0BED735E */
    uint32_t proxy_ptr;      /* +0x08  (baseline) → CTerSafeProxy+4 */
    uint32_t field_0c;       /* +0x0C  (unknown) extra ptr; observed 0x0383B964 */
} TsInitStruct;

/*
 * TsPacketWrapper — 32-byte struct passed to vt_2 ProcessPacket.  First
 * dword is the CS command id; remaining fields are uncertain.  Real TerSafe
 * does not mutate this buffer in any observed call.
 */
typedef struct TsPacketWrapper {
    uint32_t cmd;            /* +0x00  (baseline) CS cmd id (e.g. 0x0A = NET_IPINFO) */
    uint32_t field_04;       /* +0x04  (unknown) session-constant ptr */
    uint32_t field_08;       /* +0x08  (unknown) */
    uint32_t field_0c;       /* +0x0C  (unknown) */
    uint32_t field_10;       /* +0x10  (unknown) */
    uint32_t field_14;       /* +0x14  (unknown) */
    uint32_t field_18;       /* +0x18  (unknown) */
    uint32_t field_1c;       /* +0x1C  (unknown) */
} TsPacketWrapper;

_Static_assert(sizeof(TsPacketWrapper) == 32, "TsPacketWrapper must be 32 bytes");

/*
 * TsCreateObj — 256-byte object returned by CreateObj() (the main TerSafe
 * instance).  Every offset below is pre-populated by real TerSafe *before*
 * vt_0 Init runs; init_pre_state() replicates this layout.
 *
 * Legend: (pre)      populated by CreateObj allocator — we set in init_pre_state
 *         (vt_0)     written by vt_0 Init
 *         (vt_1+)    written by later vt methods — NOT applied by this stub
 */
typedef struct TsCreateObj {
    /* Three vtables (multi-inheritance layout in real TerSafe; stub aliases
     * all three to the same vtable for simplicity). */
    void    *vtable;              /* +0x00  (pre) primary vtable */
    void    *vtable_secondary;    /* +0x04  (pre) secondary vtable */
    void    *vtable_tertiary;     /* +0x08  (pre) tertiary vtable */

    /* Internal C++ sub-objects owned by real TerSafe.  Each is a class
     * instance with its own vtable inside TerSafe.dll .rdata.  Baseline
     * observation (sub-vtable trampoline hooks) shows these are called ONLY
     * from inside TerSafe itself — no external caller (CryGame / protocal-
     * handler) ever dereferences self+0x0C or self+0x10.  Since our stub
     * replaces TerSafe wholesale we never dispatch through them either;
     * NULL is safe and any accidental external deref will crash loudly
     * instead of silently hitting wrong methods on self. */
    void    *sub_obj_0c;          /* +0x0C  (pre) NULL — internal sub-object */
    void    *sub_obj_10;          /* +0x10  (pre) NULL — internal sub-object */

    /* Init-supplied context — written by vt_0. */
    uint32_t proxy_ptr;           /* +0x14  (vt_0) copy of init->proxy_ptr */
    uint32_t data_ptr;            /* +0x18  (vt_0) copy of init->data_ptr */
    uint32_t uin;                 /* +0x1C  (vt_0) copy of init->uin */

    /* Two invalid-handle sentinels. */
    uint32_t handles[2];          /* +0x20..+0x27  (pre) 0xFFFFFFFF */

    uint8_t  _pad_28[0x10];       /* +0x28..+0x37  (pre) zero */

    void    *sub_obj_38;          /* +0x38  (pre) heap sub-object */
    uint32_t _pad_3c;             /* +0x3C  (pre) zero */

    /* Startup magic constants — identical across sessions. */
    uint32_t magic_40;            /* +0x40  (pre) 0xC7003000 */
    uint32_t magic_44;            /* +0x44  (pre) 0xA2000007 */
    uint32_t field_48;            /* +0x48  (pre) zero */
    uint32_t field_4c;            /* +0x4C  (pre) zero */
    uint32_t _pad_50[2];          /* +0x50..+0x57  (pre) zero */
    uint32_t const_58;            /* +0x58  (pre) 0x0000122C */

    uint32_t _pad_5c[3];          /* +0x5C,+0x60,+0x64  (pre) zero */
    uint32_t field_68;            /* +0x68  (pre) 0x00000011 */
    uint32_t field_6c;            /* +0x6C  (pre) 0x01460000 */
    void    *field_70;            /* +0x70  (pre) heap sub-object */
    void    *field_74;            /* +0x74  (pre) heap sub-object */
    uint32_t crypto_78;           /* +0x78  (pre) 0xE5003C00 */
    uint32_t crypto_7c;           /* +0x7C  (pre) 0x871C792E */

    /* Internal-state region 0x80..0x9F — pool-tagged junk in real TerSafe,
     * harmless to encode verbatim. */
    uint32_t region_80[8];        /* +0x80..+0x9F  (pre) fixed pattern */

    /* ASCII signature "SE_ANTIBOT" followed by +0xAC = 0x08000000. */
    uint32_t sig_a0;              /* +0xA0  (pre) 0x415F4553 ('S','E','_','A') */
    uint32_t sig_a4;              /* +0xA4  (pre) 0x4249544E ('N','T','I','B') */
    uint32_t sig_a8;              /* +0xA8  (pre) 0x0000544F ('O','T','\0','\0') */
    uint32_t field_ac;            /* +0xAC  (pre) 0x08000000 */

    /* Pool-tagged state block. */
    uint32_t field_b0;            /* +0xB0  (pre) 0xB7CC9D33 */
    uint32_t field_b4;            /* +0xB4  (pre) 0xC3000007 */
    uint32_t field_b8;            /* +0xB8  (pre) 0x03000007 */
    uint32_t field_bc;            /* +0xBC  (pre) 0 */
    uint32_t field_c0;            /* +0xC0  (pre) 0x31000000 */
    uint32_t field_c4;            /* +0xC4  (pre) 0x00003030 */
    uint32_t field_c8;            /* +0xC8  (pre) 0x43000000 */
    uint32_t field_cc;            /* +0xCC  (pre) 0xDE000007 */
    uint32_t field_d0;            /* +0xD0  (pre) 0 */
    uint32_t field_d4;            /* +0xD4  (pre) 0x07000000 */
    uint32_t field_d8;            /* +0xD8  (pre) 0x3C000000 */
    uint32_t field_dc;            /* +0xDC  (pre) 0x423EBE00 */
    uint32_t field_e0;            /* +0xE0  (pre) 0x00000017 */
    uint32_t field_e4;            /* +0xE4  (pre) 0 */
    uint32_t field_e8;            /* +0xE8  (pre) 0 */
    void    *field_ec;            /* +0xEC  (pre) heap sub-object */
    uint32_t field_f0;            /* +0xF0  (pre) 0x00000093 */
    uint32_t field_f4;            /* +0xF4  (pre) 0x02750000 */
    uint32_t field_f8;            /* +0xF8  (pre) 0 */
    void    *field_fc;            /* +0xFC  (pre) heap sub-object */
} TsCreateObj;

_Static_assert(sizeof(TsCreateObj) == 256, "TsCreateObj must be 256 bytes");


/* ============================================================
 * Singleton object and vtable storage
 * ============================================================ */

static void       *g_tersafe_vtable[32];
static TsCreateObj g_tersafe_obj __attribute__((aligned(16)));


/* ============================================================
 * TerSafe CreateObj vtable — six virtual methods.
 * All declared __fastcall(ecx=self, edx=scratch, stack_args...) to match
 * MSVC __thiscall ABI.  No state-transition logic — log and return.
 * ============================================================ */

/* [0] Init — 1 stack arg, returns 0. */
static int __fastcall tersafe_vt_0_Init(
    TsCreateObj  *self,   /* ECX */
    THISCALL_EDX,
    TsInitStruct *init)   /* [esp+4] */
{
    if (init) {
        self->proxy_ptr = init->proxy_ptr;
        self->data_ptr  = init->data_ptr;
        self->uin       = init->uin;
    }
    return 0;
}

/* [1] Notify — 0 stack args, returns 0. */
static int __fastcall tersafe_vt_1_Notify(TsCreateObj *self, THISCALL_EDX) {
    (void)self;
    return 0;
}

/* [2] ProcessPacket — 1 stack arg, returns 1 always (133/133 baseline). */
static int __fastcall tersafe_vt_2_ProcessPacket(
    TsCreateObj     *self,
    THISCALL_EDX,
    TsPacketWrapper *pkt)
{
    (void)self; (void)pkt;
    return 1;
}

/* [3] QueryState — 1 stack arg, returns 0 always (163/163 baseline). */
static int __fastcall tersafe_vt_3_QueryState(
    TsCreateObj *self,
    THISCALL_EDX,
    uint32_t     arg)
{
    (void)self; (void)arg;
    return 0;
}

/* [4] Tick — unobserved in baseline.  Signature is a guess (0 stack args). */
static int __fastcall tersafe_vt_4_Tick(TsCreateObj *self, THISCALL_EDX) {
    (void)self;
    return 0;
}

/* [5] Release — 0 stack args, returns 0. */
static int __fastcall tersafe_vt_5_Release(TsCreateObj *self, THISCALL_EDX) {
    (void)self;
    return 0;
}

/* Fallback slot used for any vtable index we haven't characterised.  Takes
 * up to 4 stack args — generous envelope so stray calls don't corrupt ESP
 * on return-from-callee stack cleanup. */
static int __fastcall tersafe_vt_catch(
    TsCreateObj *self, THISCALL_EDX,
    int a0, int a1, int a2, int a3)
{
    (void)self; (void)a0; (void)a1; (void)a2; (void)a3;
    return 0;
}


/* ============================================================
 * Pre-Init state initialiser — shapes g_tersafe_obj to match what real
 * TerSafe's CreateObj() returns *before* any vt method runs.  Values are
 * lifted verbatim from the ag_mho baseline hex dump at vt_0 entry.
 * ============================================================ */
static void init_pre_state(void) {
    memset(&g_tersafe_obj, 0, sizeof g_tersafe_obj);

    /* Vtables — stub aliases all three to the same 32-slot table. */
    g_tersafe_obj.vtable           = g_tersafe_vtable;
    g_tersafe_obj.vtable_secondary = g_tersafe_vtable;
    g_tersafe_obj.vtable_tertiary  = g_tersafe_vtable;

    /* +0x0C and +0x10 are internal C++ sub-objects (own vtables) used only
     * inside real TerSafe.  Stub never dispatches through them, so NULL. */
    g_tersafe_obj.sub_obj_0c = NULL;
    g_tersafe_obj.sub_obj_10 = NULL;

    /* +0x38, +0x70, +0x74, +0xEC, +0xFC are plain heap data buffers in real
     * TerSafe (non-vtable first dwords).  Stub uses self so any in-stub
     * data read lands inside the 256-byte object. */
    g_tersafe_obj.sub_obj_38 = &g_tersafe_obj;
    g_tersafe_obj.field_70   = &g_tersafe_obj;
    g_tersafe_obj.field_74   = &g_tersafe_obj;
    g_tersafe_obj.field_ec   = &g_tersafe_obj;
    g_tersafe_obj.field_fc   = &g_tersafe_obj;

    /* Init-context slots are zero pre-Init; vt_0 fills them. */

    g_tersafe_obj.handles[0] = 0xFFFFFFFFu;
    g_tersafe_obj.handles[1] = 0xFFFFFFFFu;

    /* Invariant constants observed in every session. */
    g_tersafe_obj.magic_40   = 0xC7003000u;
    g_tersafe_obj.magic_44   = 0xA2000007u;
    g_tersafe_obj.const_58   = 0x0000122Cu;
    g_tersafe_obj.field_68   = 0x00000011u;
    g_tersafe_obj.field_6c   = 0x01460000u;
    g_tersafe_obj.crypto_78  = 0xE5003C00u;
    g_tersafe_obj.crypto_7c  = 0x871C792Eu;

    /* Pool-tagged region at +0x80..+0x9F — verbatim from baseline. */
    g_tersafe_obj.region_80[0] = 0x00000007u;
    g_tersafe_obj.region_80[1] = 0x000000ABu;
    g_tersafe_obj.region_80[2] = 0x070000E9u;
    g_tersafe_obj.region_80[3] = 0x78228893u;
    g_tersafe_obj.region_80[4] = 0x0700009Fu;
    g_tersafe_obj.region_80[5] = 0x0700000Bu;
    g_tersafe_obj.region_80[6] = 0x00000000u;
    g_tersafe_obj.region_80[7] = 0x00000055u;

    /* ASCII signature "SE_ANTIBOT" — byte layout matches real TerSafe. */
    g_tersafe_obj.sig_a0     = 0x415F4553u;   /* "S","E","_","A" */
    g_tersafe_obj.sig_a4     = 0x4249544Eu;   /* "N","T","I","B" */
    g_tersafe_obj.sig_a8     = 0x0000544Fu;   /* "O","T",0,0 */
    g_tersafe_obj.field_ac   = 0x08000000u;

    g_tersafe_obj.field_b0   = 0xB7CC9D33u;
    g_tersafe_obj.field_b4   = 0xC3000007u;
    g_tersafe_obj.field_b8   = 0x03000007u;
    g_tersafe_obj.field_bc   = 0x00000000u;
    g_tersafe_obj.field_c0   = 0x31000000u;
    g_tersafe_obj.field_c4   = 0x00003030u;
    g_tersafe_obj.field_c8   = 0x43000000u;
    g_tersafe_obj.field_cc   = 0xDE000007u;
    g_tersafe_obj.field_d0   = 0x00000000u;
    g_tersafe_obj.field_d4   = 0x07000000u;
    g_tersafe_obj.field_d8   = 0x3C000000u;
    g_tersafe_obj.field_dc   = 0x423EBE00u;
    g_tersafe_obj.field_e0   = 0x00000017u;
    g_tersafe_obj.field_e4   = 0x00000000u;
    g_tersafe_obj.field_e8   = 0x00000000u;
    g_tersafe_obj.field_f0   = 0x00000093u;
    g_tersafe_obj.field_f4   = 0x02750000u;
    g_tersafe_obj.field_f8   = 0x00000000u;
}

static void init_tersafe_vtable(void) {
    g_tersafe_vtable[0] = (void *)tersafe_vt_0_Init;
    g_tersafe_vtable[1] = (void *)tersafe_vt_1_Notify;
    g_tersafe_vtable[2] = (void *)tersafe_vt_2_ProcessPacket;
    g_tersafe_vtable[3] = (void *)tersafe_vt_3_QueryState;
    g_tersafe_vtable[4] = (void *)tersafe_vt_4_Tick;
    g_tersafe_vtable[5] = (void *)tersafe_vt_5_Release;
    for (int i = 6; i < 32; i++) g_tersafe_vtable[i] = (void *)tersafe_vt_catch;

    init_pre_state();
}


/* ============================================================
 * Exports — match tersafe.def.  Only CreateObj is called in practice.
 * ============================================================ */

__declspec(dllexport) void * __cdecl CreateObj(void) {
    return &g_tersafe_obj;
}

/* Unused exports — silent no-ops. */
__declspec(dllexport) int __stdcall DllEntry1(void *init) { (void)init; return 0; }
__declspec(dllexport) int __cdecl DllEntry2(void) { return 0; }
__declspec(dllexport) int __cdecl DllEntry3(void) { return 0; }
__declspec(dllexport) int __cdecl DllEntry4(void) { return 0; }
__declspec(dllexport) int __cdecl DllEntry5(void) { return 0; }
__declspec(dllexport) int __cdecl DllEntry6(void) { return 0; }
__declspec(dllexport) int __cdecl DllEntry7(void) { return 0; }
__declspec(dllexport) void * __cdecl DllEntry8(void) { return NULL; }
__declspec(dllexport) void * __cdecl DllEntry (void) { return NULL; }


/* ============================================================
 * DllMain — initialise singleton only.
 * ============================================================ */

BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, LPVOID reserved) {
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(h);
        init_tersafe_vtable();
    }
    return TRUE;
}
