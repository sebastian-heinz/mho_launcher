#include "hooks_crygame.h"
#include "hooks_mhoclient.h"  // server_url_address
#include "log.h"
#include "hook_table.h"
#include "ag_ini.h"
#include "win_util.h"

#include "mho_types.h"
#include "cs_cmd.h"
#include "internal_notification_ids.h"

#include <cstdint>
#include <cstring>
#include <string>

// Module bases — set during install_crygame_hooks for backtrace formatting.
static DWORD crygame_base = 0;
static DWORD mhoclient_base_bt = 0;
static DWORD tenproxy_base_bt = 0;
static DWORD protocal_base_bt = 0;
static DWORD msvcr120_base_bt = 0;
static DWORD ntdll_base_bt = 0;
static DWORD kernel32_base_bt = 0;

// Key global offsets (relative to CryGame.dll base = 0x10000000)
static constexpr DWORD OFF_DAT_12387c6c         = 0x2387c6c;  // CStaticDataEnv pointer
static constexpr DWORD OFF_DAT_12387c70         = 0x2387c70;  // CStaticDataEnv init flag
static constexpr DWORD OFF_DAT_12387f48         = 0x2387f48;  // CMonsterInfo static default
static constexpr DWORD OFF_g_CMonsterInfo_Ptr   = 0x23bee60;  // g_CMonsterInfo_SingletonPtr
static constexpr DWORD OFF_g_pCGameRules        = 0x20286b8;
static constexpr DWORD OFF_g_pCBattleGround     = 0x20286c8;

static constexpr DWORD OFF_ENTITY_MONSTER_INFO  = 0x1250;
static constexpr DWORD OFF_MONSTER_INFO_TYPE    = 0x20;

static constexpr DWORD OFF_CMD_714_HANDLER      = 0x129f8b0;
static constexpr DWORD OFF_SPAWN_MONSTERS       = 0x12a2e10;
static constexpr DWORD OFF_CMD_663_HANDLER      = 0x12a3ac0;
static constexpr DWORD OFF_CMD_662_HANDLER      = 0x12a5580;
static constexpr DWORD OFF_CMD_730_HANDLER      = 0x1289a00;
static constexpr DWORD OFF_CMD_720_HANDLER      = 0x12e3be0;
static constexpr DWORD OFF_ENTITY_JOIN_WRAPPER  = 0x12ad180;
static constexpr DWORD OFF_PROTOCOL_DISPATCH    = 0x12230c0;
static constexpr DWORD OFF_PROTOCOL_ON_RECV_MSG = 0x12408f0;
static constexpr DWORD OFF_ENTITY_JOIN_INNER    = 0x140d730;
static constexpr DWORD OFF_INIT_FROM_APPEAR     = 0x13a3720;
static constexpr DWORD OFF_GET_MONSTER_INFO     = 0x13a23b0;
static constexpr DWORD OFF_SET_MONSTER_INFO     = 0x13a3da0;

static constexpr int MONSTER_INFO_TYPE_MIN = 0;
static constexpr int MONSTER_INFO_TYPE_MAX = 10;

static constexpr uint32_t TRACKED_MONSTER_NET_ID = 0x10000001;

static constexpr DWORD OFF_APPEAR_FACTION        = 0x6E;
static constexpr DWORD OFF_APPEAR_BTSTATE        = 0x72;
static constexpr DWORD OFF_APPEAR_DEAD           = 0xAB4;
static constexpr DWORD OFF_APPEAR_LCM            = 0xAB5;
static constexpr DWORD OFF_APPEAR_PARENT_GUID    = 0x42DF;
static constexpr DWORD OFF_APPEAR_LAST_CHILD_ID  = 0x42E7;

static constexpr DWORD OFF_LCM_SYNC_TIME         = OFF_APPEAR_LCM + 0x01;
static constexpr DWORD OFF_LCM_MONSTER_ID        = OFF_APPEAR_LCM + 0x09;
static constexpr DWORD OFF_LCM_ANIM_SEQ_NAME     = OFF_APPEAR_LCM + 0x0D;
static constexpr DWORD OFF_LCM_SKILL_ID          = OFF_APPEAR_LCM + 0x12B;
static constexpr DWORD OFF_LCM_SKILL_LV          = OFF_APPEAR_LCM + 0x12F;
static constexpr DWORD OFF_LCM_TARGET_ID         = OFF_APPEAR_LCM + 0x137;
static constexpr DWORD OFF_LCM_FLAG              = OFF_APPEAR_LCM + 0x13F;
static constexpr DWORD OFF_LCM_MOVE_SPEED        = OFF_APPEAR_LCM + 0x14F;
static constexpr DWORD OFF_LCM_MONSTER_POS       = OFF_APPEAR_LCM + 0x174;
static constexpr DWORD OFF_LCM_MONSTER_ROT       = OFF_APPEAR_LCM + 0x180;
static constexpr DWORD OFF_LCM_SKILL_SPEED       = OFF_APPEAR_LCM + 0x190;
static constexpr DWORD OFF_LCM_RESTART_ANIM      = OFF_APPEAR_LCM + 0x194;
static constexpr DWORD OFF_LCM_TARGET_ATTACK_NUM = OFF_APPEAR_LCM + 0x199;
static constexpr DWORD OFF_LCM_SET_ROTATE        = OFF_APPEAR_LCM + 0x216;
static constexpr DWORD OFF_LCM_SET_POS           = OFF_APPEAR_LCM + 0x217;
static constexpr DWORD OFF_LCM_NEED_MOVE_SPEED_ACC = OFF_APPEAR_LCM + 0x219;

// --- original function pointers ---

static fn_crygame_13EC290 org_fn_crygame_13EC290 = nullptr;

// --- helpers ---

template <typename T>
static T read_unaligned(const uint8_t *ptr) {
    T value{};
    std::memcpy(&value, ptr, sizeof(value));
    return value;
}

static bool safe_read_bytes(uint32_t address, void *out, size_t size) {
    if ((address < 0x10000) || (out == nullptr) || (size == 0)) {
        return false;
    }

    SIZE_T bytes_read = 0;
    return ReadProcessMemory(
               GetCurrentProcess(),
               reinterpret_cast<LPCVOID>(static_cast<uintptr_t>(address)),
               out,
               size,
               &bytes_read) != 0 &&
           bytes_read == size;
}

template <typename T>
static bool safe_read_value(uint32_t address, T *out) {
    if (out == nullptr) {
        return false;
    }

    T value{};
    bool ok = safe_read_bytes(address, &value, sizeof(value));
    *out = value;
    return ok;
}

static const CsCmd *find_cmd(uint32_t packet_id) {
    for (int i = 0; i < CMDS_SIZE; i++) {
        if (CMDS[i].id == packet_id) {
            return &CMDS[i];
        }
    }
    return nullptr;
}

static const char *cmd_name(uint32_t packet_id) {
    const CsCmd *cmd = find_cmd(packet_id);
    return cmd ? cmd->name.c_str() : "UNKNOWN";
}

static bool is_spawn_cmd(uint32_t packet_id) {
    switch (packet_id) {
        case 528:
        case 533:
        case 534:
        case 641:
        case 648:
        case 650:
        case 651:
        case 662:
        case 663:
        case 714:
        case 720:
        case 730:
            return true;
        default:
            return false;
    }
}

static DWORD relative_cry_offset(uint32_t address) {
    if ((crygame_base == 0) || (address < crygame_base)) {
        return 0;
    }
    return address - crygame_base;
}

// Format an address as "<module>+0x<offset>" into `buf`, picking whichever
// module base it lies closest to. If it's outside all known ranges we just
// print the raw address. Buffer should be ≥32 bytes.
static void format_module_offset(uint32_t addr, char *buf, size_t buf_sz) {
    struct M { const char *name; DWORD base; DWORD span; };
    static const DWORD SPAN_BIG   = 0x02000000;  // for CryGame / MHOClient (big images)
    static const DWORD SPAN_SMALL = 0x00400000;  // for tenproxy / protocalhandler / CRT / ntdll
    M modules[] = {
        {"cry",   crygame_base,      SPAN_BIG  },
        {"mho",   mhoclient_base_bt, SPAN_BIG  },
        {"tp",    tenproxy_base_bt,  SPAN_SMALL},
        {"ph",    protocal_base_bt,  SPAN_SMALL},
        {"msvcr", msvcr120_base_bt,  SPAN_SMALL},
        {"ntdll", ntdll_base_bt,     SPAN_SMALL},
        {"k32",   kernel32_base_bt,  SPAN_SMALL},
    };
    for (size_t i = 0; i < sizeof(modules) / sizeof(modules[0]); i++) {
        M &m = modules[i];
        if ((m.base != 0) && (addr >= m.base) && (addr < m.base + m.span)) {
            snprintf(buf, buf_sz, "%s+0x%X", m.name, addr - m.base);
            return;
        }
    }
    snprintf(buf, buf_sz, "?+0x%X", addr);
}

// Refresh module bases — tenproxy.dll and protocalhandler.dll load AFTER
// install_crygame_hooks runs, so their bases were 0 at install time. Called at
// the start of every backtrace so addresses in those modules get formatted
// with the right prefix once they're loaded.
static void refresh_backtrace_modules() {
    if (mhoclient_base_bt == 0) mhoclient_base_bt = (DWORD)GetModuleHandleA("MHOClient.exe");
    if (tenproxy_base_bt  == 0) tenproxy_base_bt  = (DWORD)GetModuleHandleA("tenproxy.dll");
    if (protocal_base_bt  == 0) protocal_base_bt  = (DWORD)GetModuleHandleA("protocalhandler.dll");
    if (msvcr120_base_bt  == 0) msvcr120_base_bt  = (DWORD)GetModuleHandleA("MSVCR120.dll");
    if (ntdll_base_bt     == 0) ntdll_base_bt     = (DWORD)GetModuleHandleA("ntdll.dll");
    if (kernel32_base_bt  == 0) kernel32_base_bt  = (DWORD)GetModuleHandleA("kernel32.dll");
}

// Walks the `ebp` frame-pointer chain starting from ctx->ebp and logs up to
// `max_depth` upstream frames. Stops early if we hit unreadable memory, a
// monotonic break (ebp doesn't grow), or a return address that doesn't look
// like executable code. Works only when callees preserve frame pointers; any
// frame compiled with -fomit-frame-pointer breaks the chain beyond that point
// — we detect via sanity checks and stop.
static void log_backtrace(HookContext *ctx, const char *tag, int max_depth) {
    refresh_backtrace_modules();
    char fmt[48];
    DWORD *stk = hook_stack(ctx);
    uint32_t ret0 = stk[0];
    format_module_offset(ret0, fmt, sizeof(fmt));
    log("[%s.bt] f0 ret=%p %s\n", tag, (void *)ret0, fmt);

    uint32_t ebp = ctx->ebp;
    for (int i = 1; i <= max_depth; i++) {
        // Sanity: ebp must look like a user-space stack pointer.
        if ((ebp < 0x10000) || (ebp > 0xC0000000)) break;
        uint32_t saved_ebp = 0, saved_ret = 0;
        if (!safe_read_value(ebp, &saved_ebp))     break;
        if (!safe_read_value(ebp + 4, &saved_ret)) break;
        if (saved_ret < 0x10000) break;  // not a plausible code address
        format_module_offset(saved_ret, fmt, sizeof(fmt));
        log("[%s.bt] f%d ret=%p %s\n", tag, i, (void *)saved_ret, fmt);
        // Next-frame ebp must strictly grow upward; otherwise FPO or garbage.
        if ((saved_ebp <= ebp) || (saved_ebp > 0xC0000000)) break;
        ebp = saved_ebp;
    }
}

static const char *classify_spawn_caller(uint32_t return_address) {
    DWORD rel = relative_cry_offset(return_address);
    if ((rel >= OFF_CMD_714_HANDLER) && (rel < OFF_CMD_714_HANDLER + 0x400)) {
        return "CMD714";
    }
    if ((rel >= OFF_SPAWN_MONSTERS) && (rel < OFF_SPAWN_MONSTERS + 0x500)) {
        return "SpawnMonsters";
    }
    if ((rel >= OFF_CMD_663_HANDLER) && (rel < OFF_CMD_663_HANDLER + 0x300)) {
        return "CMD663";
    }
    if ((rel >= OFF_CMD_662_HANDLER) && (rel < OFF_CMD_662_HANDLER + 0x400)) {
        return "CMD662";
    }
    if ((rel >= OFF_CMD_730_HANDLER) && (rel < OFF_CMD_730_HANDLER + 0x120)) {
        return "CMD730";
    }
    if ((rel >= OFF_CMD_720_HANDLER) && (rel < OFF_CMD_720_HANDLER + 0x120)) {
        return "CMD720";
    }
    if ((rel >= OFF_ENTITY_JOIN_WRAPPER) && (rel < OFF_ENTITY_JOIN_WRAPPER + 0x80)) {
        return "EntityJoinWrapper";
    }
    if ((rel >= OFF_ENTITY_JOIN_INNER) && (rel < OFF_ENTITY_JOIN_INNER + 0x900)) {
        return "EntityJoin";
    }
    if ((rel >= OFF_INIT_FROM_APPEAR) && (rel < OFF_INIT_FROM_APPEAR + 0x400)) {
        return "InitFromAppearNtf";
    }
    if ((rel >= OFF_SET_MONSTER_INFO) && (rel < OFF_SET_MONSTER_INFO + 0x80)) {
        return "SetMonsterInfo";
    }
    if ((rel >= OFF_GET_MONSTER_INFO) && (rel < OFF_GET_MONSTER_INFO + 0x80)) {
        return "GetMonsterInfo";
    }
    return "other";
}

static bool is_interesting_spawn_caller(uint32_t return_address) {
    return std::strcmp(classify_spawn_caller(return_address), "other") != 0;
}

static bool monster_type_valid(int32_t type) {
    return (type >= MONSTER_INFO_TYPE_MIN) && (type <= MONSTER_INFO_TYPE_MAX);
}

static void log_runtime_globals(const char *tag) {
    uint32_t static_env = 0;
    uint32_t env_flag = 0;
    uint32_t moninfo_ptr = 0;
    uint32_t bg_ptr = 0;
    uint32_t gr_ptr = 0;
    uint32_t gr_78 = 0;

    safe_read_value(crygame_base + OFF_DAT_12387c6c, &static_env);
    safe_read_value(crygame_base + OFF_DAT_12387c70, &env_flag);
    safe_read_value(crygame_base + OFF_g_CMonsterInfo_Ptr, &moninfo_ptr);
    safe_read_value(crygame_base + OFF_g_pCBattleGround, &bg_ptr);
    safe_read_value(crygame_base + OFF_g_pCGameRules, &gr_ptr);
    if (gr_ptr >= 0x10000) {
        safe_read_value(gr_ptr + 0x78, &gr_78);
    }

    log("[%s] globals env=%p flag=%u CMonsterInfo=%p BG=%p GR=%p GR+78=%p\n",
        tag,
        (void *)static_env,
        env_flag,
        (void *)moninfo_ptr,
        (void *)bg_ptr,
        (void *)gr_ptr,
        (void *)gr_78);
}

static void log_game_logic_state(const char *tag, uint32_t this_ptr) {
    uint32_t owner = 0;
    uint32_t owner_vft = 0;
    uint32_t battle_78 = 0;
    uint32_t owner_90 = 0;
    bool have_owner = safe_read_value(this_ptr + 0x0c, &owner);
    bool have_owner_vft = have_owner && safe_read_value(owner, &owner_vft);
    bool have_battle_78 = (owner >= 0x10000) && safe_read_value(owner + 0x78, &battle_78);
    bool have_owner_90 = (owner >= 0x10000) && safe_read_value(owner + 0x90, &owner_90);

    log("[%s] this=%p this+0xc=%p owner_vft=%p +0x78=%p +0x90=%p ok(owner=%d vft=%d 78=%d 90=%d)\n",
        tag,
        (void *)this_ptr,
        (void *)owner,
        (void *)owner_vft,
        (void *)battle_78,
        (void *)owner_90,
        have_owner ? 1 : 0,
        have_owner_vft ? 1 : 0,
        have_battle_78 ? 1 : 0,
        have_owner_90 ? 1 : 0);
}

static void log_packet_preview(const char *tag, uint32_t packet_ptr) {
    uint8_t preview[16] = {};
    bool ok = safe_read_bytes(packet_ptr, preview, sizeof(preview));
    if (!ok) {
        log("[%s] packet=%p preview:unreadable\n", tag, (void *)packet_ptr);
        return;
    }

    log("[%s] packet=%p bytes=%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
        tag,
        (void *)packet_ptr,
        preview[0], preview[1], preview[2], preview[3],
        preview[4], preview[5], preview[6], preview[7],
        preview[8], preview[9], preview[10], preview[11],
        preview[12], preview[13], preview[14], preview[15]);
}

static bool is_tracked_monster_net_id(uint32_t net_id) {
    return net_id == TRACKED_MONSTER_NET_ID;
}

static void log_protocol_handler_state(const char *tag, uint32_t this_ptr) {
    uint32_t owner = 0;
    uint32_t callback = 0;
    bool have_owner = safe_read_value(this_ptr + 0x04, &owner);
    bool have_callback = safe_read_value(this_ptr + 0x08, &callback);

    log("[%s] proto_handler this=%p this+4(owner):%p this+8(callback):%p ok(owner=%d callback=%d)\n",
        tag,
        (void *)this_ptr,
        (void *)owner,
        (void *)callback,
        have_owner ? 1 : 0,
        have_callback ? 1 : 0);
}

static void log_handler_definition_snapshot(const char *tag, uint32_t handler_def_ptr) {
    HandlerCallbackDefintion handler_def{};
    bool ok = safe_read_bytes(handler_def_ptr, &handler_def, sizeof(handler_def));
    if (!ok) {
        log("[%s] def=%p snapshot:unreadable\n", tag, (void *)handler_def_ptr);
        return;
    }

    log("[%s] def=%p vft=%p mgr=%p fn=%p unk=%u\n",
        tag,
        (void *)handler_def_ptr,
        (void *)handler_def.vftable_ptr,
        (void *)handler_def.mgr_instance_ptr,
        handler_def.handler_callback_function_ptr,
        handler_def.unknown_field);
}

static uint32_t protocol_dispatch_find_handler(uint32_t dispatch_ptr, uint32_t packet_id, uint32_t *node_out) {
    uint32_t root = 0;
    if (!safe_read_value(dispatch_ptr + 0x0c, &root)) {
        return 0;
    }

    uint32_t tree_end = dispatch_ptr + 0x08;
    uint32_t candidate = tree_end;
    while (root != 0) {
        uint32_t key = 0;
        if (!safe_read_value(root + 0x10, &key)) {
            break;
        }

        uint32_t next = 0;
        if (key < packet_id) {
            if (!safe_read_value(root + 0x0c, &next)) {
                break;
            }
        } else {
            if (!safe_read_value(root + 0x08, &next)) {
                break;
            }
            candidate = root;
        }
        root = next;
    }

    if ((node_out != nullptr) && (candidate != tree_end)) {
        *node_out = candidate;
    }

    if (candidate == tree_end) {
        return 0;
    }

    uint32_t key = 0;
    if (!safe_read_value(candidate + 0x10, &key) || (packet_id < key)) {
        return 0;
    }

    uint32_t handler_def = 0;
    if (!safe_read_value(candidate + 0x14, &handler_def)) {
        return 0;
    }
    return handler_def;
}

static void log_monster_appear_entry(const char *tag, const uint8_t *entry, bool controlled) {
    if (entry == nullptr) {
        log("[%s] entry:NULL\n", tag);
        return;
    }

    int32_t net_id = read_unaligned<int32_t>(entry + 0x00);
    int16_t spawn_type = read_unaligned<int16_t>(entry + 0x04);
    int32_t monster_info_id = read_unaligned<int32_t>(entry + 0x06);
    float scale = read_unaligned<float>(entry + 0x40);
    uint8_t dead_flag = read_unaligned<uint8_t>(entry + OFF_APPEAR_DEAD);
    int32_t faction = read_unaligned<int32_t>(entry + OFF_APPEAR_FACTION);
    uint64_t parent_guid = read_unaligned<uint64_t>(entry + OFF_APPEAR_PARENT_GUID);
    int32_t last_child_id = read_unaligned<int32_t>(entry + OFF_APPEAR_LAST_CHILD_ID);

    float pose_qx = read_unaligned<float>(entry + 0x52);
    float pose_qy = read_unaligned<float>(entry + 0x56);
    float pose_qz = read_unaligned<float>(entry + 0x5A);
    float pose_qw = read_unaligned<float>(entry + 0x5E);
    float pose_x = read_unaligned<float>(entry + 0x62);
    float pose_y = read_unaligned<float>(entry + 0x66);
    float pose_z = read_unaligned<float>(entry + 0x6A);

    long long lcm_sync_time = read_unaligned<long long>(entry + OFF_LCM_SYNC_TIME);
    uint32_t lcm_monster_id = read_unaligned<uint32_t>(entry + OFF_LCM_MONSTER_ID);
    int32_t lcm_skill_id = read_unaligned<int32_t>(entry + OFF_LCM_SKILL_ID);
    int32_t lcm_skill_lv = read_unaligned<int32_t>(entry + OFF_LCM_SKILL_LV);
    uint32_t lcm_target_id = read_unaligned<uint32_t>(entry + OFF_LCM_TARGET_ID);
    uint32_t lcm_flag = read_unaligned<uint32_t>(entry + OFF_LCM_FLAG);
    float move_x = read_unaligned<float>(entry + OFF_LCM_MOVE_SPEED + 0x0);
    float move_y = read_unaligned<float>(entry + OFF_LCM_MOVE_SPEED + 0x4);
    float move_z = read_unaligned<float>(entry + OFF_LCM_MOVE_SPEED + 0x8);
    float lcm_pos_x = read_unaligned<float>(entry + OFF_LCM_MONSTER_POS + 0x0);
    float lcm_pos_y = read_unaligned<float>(entry + OFF_LCM_MONSTER_POS + 0x4);
    float lcm_pos_z = read_unaligned<float>(entry + OFF_LCM_MONSTER_POS + 0x8);
    float lcm_rot_x = read_unaligned<float>(entry + OFF_LCM_MONSTER_ROT + 0x0);
    float lcm_rot_y = read_unaligned<float>(entry + OFF_LCM_MONSTER_ROT + 0x4);
    float lcm_rot_z = read_unaligned<float>(entry + OFF_LCM_MONSTER_ROT + 0x8);
    float lcm_rot_w = read_unaligned<float>(entry + OFF_LCM_MONSTER_ROT + 0xC);
    float lcm_skill_speed = read_unaligned<float>(entry + OFF_LCM_SKILL_SPEED);
    uint8_t lcm_restart_anim = read_unaligned<uint8_t>(entry + OFF_LCM_RESTART_ANIM);
    int32_t lcm_target_attack_num = read_unaligned<int32_t>(entry + OFF_LCM_TARGET_ATTACK_NUM);
    uint8_t lcm_set_rotate = read_unaligned<uint8_t>(entry + OFF_LCM_SET_ROTATE);
    uint8_t lcm_set_pos = read_unaligned<uint8_t>(entry + OFF_LCM_SET_POS);
    uint8_t lcm_need_move_speed_acc = read_unaligned<uint8_t>(entry + OFF_LCM_NEED_MOVE_SPEED_ACC);

    log("[%s] entry NetId=%d SpawnType=%d MonsterInfoId=%d Name=%.32s Class=%.32s Scale=%.2f Dead=%u\n",
        tag,
        net_id,
        (int)spawn_type,
        monster_info_id,
        reinterpret_cast<const char *>(entry + 0x12),
        reinterpret_cast<const char *>(entry + 0x32),
        scale,
        (unsigned)dead_flag);
    log("[%s] pose t=(%.2f,%.2f,%.2f) q=(%.2f,%.2f,%.2f,%.2f) faction=%d BTState=%.32s ParentGuid=%llu LastChild=%d\n",
        tag,
        pose_x, pose_y, pose_z,
        pose_qx, pose_qy, pose_qz, pose_qw,
        faction,
        reinterpret_cast<const char *>(entry + OFF_APPEAR_BTSTATE),
        static_cast<unsigned long long>(parent_guid),
        last_child_id);
    log("[%s] lcm id=%u sync=%lld anim=%.32s skill=%d/%d target=%u flag=%u move=(%.2f,%.2f,%.2f) pos=(%.2f,%.2f,%.2f) rot=(%.2f,%.2f,%.2f,%.2f) speed=%.2f restart=%u atkNum=%d setRot=%u setPos=%u accel=%u\n",
        tag,
        lcm_monster_id,
        lcm_sync_time,
        reinterpret_cast<const char *>(entry + OFF_LCM_ANIM_SEQ_NAME),
        lcm_skill_id,
        lcm_skill_lv,
        lcm_target_id,
        lcm_flag,
        move_x, move_y, move_z,
        lcm_pos_x, lcm_pos_y, lcm_pos_z,
        lcm_rot_x, lcm_rot_y, lcm_rot_z, lcm_rot_w,
        lcm_skill_speed,
        (unsigned)lcm_restart_anim,
        lcm_target_attack_num,
        (unsigned)lcm_set_rotate,
        (unsigned)lcm_set_pos,
        (unsigned)lcm_need_move_speed_acc);

    if (controlled) {
        int32_t owner_id = read_unaligned<int32_t>(entry + 0x42EB);
        int32_t control_type = read_unaligned<int32_t>(entry + 0x42EF);
        float duration = read_unaligned<float>(entry + 0x42F3);
        log("[%s] controlled OwnerId=%d Type=%d Duration=%.2f\n",
            tag,
            owner_id,
            control_type,
            duration);
    }
}

static void log_entity_monster_info(const char *tag, uint32_t entity_ptr, uint32_t return_address) {
    uint32_t entity_vft = 0;
    uint32_t entity_field4 = 0;
    uint32_t info_ptr = 0;
    uint32_t info_vft = 0;
    int32_t monster_type = -1;

    bool have_entity_vft = safe_read_value(entity_ptr, &entity_vft);
    bool have_entity_field4 = safe_read_value(entity_ptr + 0x04, &entity_field4);
    bool have_info_ptr = safe_read_value(entity_ptr + OFF_ENTITY_MONSTER_INFO, &info_ptr);
    bool have_info_vft = (info_ptr >= 0x10000) && safe_read_value(info_ptr, &info_vft);
    bool have_type = (info_ptr >= 0x10000) && safe_read_value(info_ptr + OFF_MONSTER_INFO_TYPE, &monster_type);
    bool is_default_info = info_ptr == (crygame_base + OFF_DAT_12387f48);

    log("[%s] caller=%s ret=%p cry+0x%08X entity=%p evft=%p e+4=%u info=%p ivft=%p type=%d ok(evft=%d e+4=%d info=%d ivft=%d type=%d) default=%d\n",
        tag,
        classify_spawn_caller(return_address),
        (void *)return_address,
        (unsigned)relative_cry_offset(return_address),
        (void *)entity_ptr,
        (void *)entity_vft,
        entity_field4,
        (void *)info_ptr,
        (void *)info_vft,
        monster_type,
        have_entity_vft ? 1 : 0,
        have_entity_field4 ? 1 : 0,
        have_info_ptr ? 1 : 0,
        have_info_vft ? 1 : 0,
        have_type ? 1 : 0,
        is_default_info ? 1 : 0);
}

static void log_set_monster_info(const char *tag, uint32_t entity_ptr, uint32_t info_ptr, uint32_t return_address) {
    uint32_t entity_vft = 0;
    uint32_t info_vft = 0;
    int32_t monster_type = -1;

    bool have_entity_vft = safe_read_value(entity_ptr, &entity_vft);
    bool have_info_vft = (info_ptr >= 0x10000) && safe_read_value(info_ptr, &info_vft);
    bool have_type = (info_ptr >= 0x10000) && safe_read_value(info_ptr + OFF_MONSTER_INFO_TYPE, &monster_type);
    bool is_default_info = info_ptr == (crygame_base + OFF_DAT_12387f48);

    log("[%s] caller=%s ret=%p cry+0x%08X entity=%p evft=%p info_arg=%p ivft=%p type=%d ok(evft=%d ivft=%d type=%d) default=%d\n",
        tag,
        classify_spawn_caller(return_address),
        (void *)return_address,
        (unsigned)relative_cry_offset(return_address),
        (void *)entity_ptr,
        (void *)entity_vft,
        (void *)info_ptr,
        (void *)info_vft,
        monster_type,
        have_entity_vft ? 1 : 0,
        have_info_vft ? 1 : 0,
        have_type ? 1 : 0,
        is_default_info ? 1 : 0);
}

// --- handler callbacks (called from hook context wrappers below) ---

static void __cdecl crygame_13F3640() {
    log("crygame_13F3640\n");
    const char *url = "127.0.0.1:8142";
    WriteMemory((LPVOID) server_url_address, url, strlen(url));
    org_fn_crygame_13EC290();
}

static void register_handler(uint32_t packet_id, HandlerCallbackDefintion *handler_definition,
                             uint32_t return_address, uint32_t this_ptr) {
    if (handler_definition != nullptr) {
        HandlerCallbackDefintion snapshot{};
        bool ok = safe_read_bytes(reinterpret_cast<uint32_t>(handler_definition), &snapshot, sizeof(snapshot));
        if (ok) {
            log("register_handler CMD=%u %s ret=%p cry+0x%08X ecx=%p def=%p vft=%p mgr=%p fn=%p unk=%u\n",
                packet_id,
                cmd_name(packet_id),
                (void *)return_address,
                (unsigned)relative_cry_offset(return_address),
                (void *)this_ptr,
                handler_definition,
                (void *)snapshot.vftable_ptr,
                (void *)snapshot.mgr_instance_ptr,
                snapshot.handler_callback_function_ptr,
                snapshot.unknown_field);
        } else {
            log("register_handler CMD=%u %s ret=%p cry+0x%08X ecx=%p def=%p snapshot:unreadable\n",
                packet_id,
                cmd_name(packet_id),
                (void *)return_address,
                (unsigned)relative_cry_offset(return_address),
                (void *)this_ptr,
                handler_definition);
        }
    } else {
        log("register_handler CMD=%u %s ret=%p cry+0x%08X ecx=%p def:NULL\n",
            packet_id,
            cmd_name(packet_id),
            (void *)return_address,
            (unsigned)relative_cry_offset(return_address),
            (void *)this_ptr);
    }
}

static void call_handler(void *call_fn, HandlerCallbackDefintion *handler_definition,
                         uint32_t packet_id, uint8_t *packet_data) {
    if (handler_definition != nullptr) {
        HandlerCallbackDefintion snapshot{};
        bool ok = safe_read_bytes(reinterpret_cast<uint32_t>(handler_definition), &snapshot, sizeof(snapshot));
        if (ok) {
            log("call_handler CMD=%u %s def=%p vft=%p mgr=%p fn=%p unk=%u call_fn=%p packet=%p\n",
                packet_id,
                cmd_name(packet_id),
                handler_definition,
                (void *)snapshot.vftable_ptr,
                (void *)snapshot.mgr_instance_ptr,
                snapshot.handler_callback_function_ptr,
                snapshot.unknown_field,
                call_fn,
                packet_data);
        } else {
            log("call_handler CMD=%u %s def=%p snapshot:unreadable call_fn=%p packet=%p\n",
                packet_id,
                cmd_name(packet_id),
                handler_definition,
                call_fn,
                packet_data);
        }
    } else {
        log("call_handler CMD=%u %s call_fn=%p packet=%p\n",
            packet_id,
            cmd_name(packet_id),
            call_fn,
            packet_data);
    }
}

static void log_handle_notification(char *event_cstr, InternalEventNotification *notif,
                                     uint32_t unk_group_index) {
    std::string notification_type = "unknown";
    std::string event = event_cstr;
    if (event.find("handleLogicNotification") != std::string::npos) {
        notification_type = GetMHLogicEventIDName(static_cast<MHLogicEventID>(notif->notification_id));
    } else if (event.find("handleGameNotification") != std::string::npos) {
        notification_type = GetMHGameEventIDName(static_cast<MHGameEventID>(notif->notification_id));
    }
    log("log_handle_logic_or_game_event_notification(event=%s, notification_id=%u (type: %s), unk_group_idx: %u)\n",
        event_cstr, notif->notification_id, notification_type.c_str(), unk_group_index);
}

// --- hook context callbacks ---

static void on_register_handler(HookContext *ctx) {
    // Original asm read from [esp+0x28] and [esp+0x2C] (= stack args at +0x8 and +0xC past pushad)
    DWORD *stack = hook_stack(ctx);
    uint32_t packet_id = stack[2];
    HandlerCallbackDefintion *handler_def = (HandlerCallbackDefintion *)stack[3];
    register_handler(packet_id, handler_def, stack[0], ctx->ecx);
}

static void on_protocol_handler_recv_msg(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t packet_ptr = stack[1];
    if (packet_ptr < 0x10000) {
        return;
    }

    uint32_t body_ptr = packet_ptr + 0x10;
    uint16_t packet_id = 0;
    if (!safe_read_value(body_ptr, &packet_id) || !is_spawn_cmd(packet_id)) {
        return;
    }

    log("[OnRecvMsg] ret=%p ecx=%p packet=%p body=%p cmd=%u %s\n",
        (void *)stack[0],
        (void *)ctx->ecx,
        (void *)packet_ptr,
        (void *)body_ptr,
        (unsigned)packet_id,
        cmd_name(packet_id));
    log_protocol_handler_state("OnRecvMsg", ctx->ecx);
    log_packet_preview("OnRecvMsg", body_ptr);
}

static void on_protocol_dispatch_entry(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t packet_ptr = stack[2];
    if (packet_ptr < 0x10000) {
        return;
    }

    uint16_t packet_id = 0;
    if (!safe_read_value(packet_ptr, &packet_id) || !is_spawn_cmd(packet_id)) {
        return;
    }

    uint32_t root = 0;
    safe_read_value(ctx->ecx + 0x0c, &root);
    uint32_t node = 0;
    uint32_t handler_def = protocol_dispatch_find_handler(ctx->ecx, packet_id, &node);

    log("[DispatchEntry] ret=%p ecx=%p arg1=%p packet=%p cmd=%u %s root=%p node=%p def=%p found=%d\n",
        (void *)stack[0],
        (void *)ctx->ecx,
        (void *)stack[1],
        (void *)packet_ptr,
        (unsigned)packet_id,
        cmd_name(packet_id),
        (void *)root,
        (void *)node,
        (void *)handler_def,
        handler_def != 0 ? 1 : 0);
    log_packet_preview("DispatchEntry", packet_ptr);
    if (handler_def != 0) {
        log_handler_definition_snapshot("DispatchEntry", handler_def);
    }
}

// --- Path B runtime resolver ---------------------------------------------------
// Given a CMD id, compute (dispatcher_stub_va, subscriber_list_offset) at runtime
// by walking CryGame's compact jump tables. No hardcoded per-CMD table — every
// CMD is automatically covered.
//
// CGameLogic::CmdDispatchSwitch @ crygame+0x65ede0 implements two ranges:
//
//   Range 1: 1         <= cmd <= 0x201
//            selector @ crygame+0x6639ec  (0x10e bytes, one per cmd-1)
//            jmptbl   @ crygame+0x6637b4  (dword entries)
//
//   Range 2: 0x202     <= cmd <= 0x501        (covers all battle/monster CMDs)
//            selector @ crygame+0x663e3c  (0x208 bytes, one per cmd-0x202)
//            jmptbl   @ crygame+0x663afc  (dword entries)
//
// Each jmptbl entry is a "thunk": `lea eax,[edx+0x10]; mov [ebp+8],eax; pop ebp;
// jmp <dispatch_stub>`. The stub's prologue is always:
//   push ebp; mov ebp,esp; push ebx; push esi;
//   mov esi, [ecx + LIST_OFFSET];
//   push edi; lea edi, [ecx + LIST_OFFSET];
// We read LIST_OFFSET from the prologue.
//
// Note on registration hooks (Path B parity question): the per-list register
// functions are NOT a single helper — there are ~280 separate register functions
// (one per list offset, each doing its own intrusive insertion). Hooking all of
// them is not practical. Instead we approximate registration visibility by
// diffing the subscriber set across consecutive dispatches of the same CMD —
// see `path_b_diff_*` below. Caveat: register/unregister cycles between two
// consecutive dispatches of the same CMD are missed.

static bool path_b_resolve(uint32_t cmd, uint32_t *stub_va_out, uint32_t *list_offset_out) {
    if (crygame_base == 0) return false;
    uint32_t cmd_lo, sel_va, jmp_va, sel_size;
    if ((cmd >= 1) && (cmd <= 0x201)) {
        cmd_lo = 1;
        sel_va = crygame_base + 0x6639ec;
        jmp_va = crygame_base + 0x6637b4;
        sel_size = 0x10e;
    } else if ((cmd >= 0x202) && (cmd <= 0x501)) {
        cmd_lo = 0x202;
        sel_va = crygame_base + 0x663e3c;
        jmp_va = crygame_base + 0x663afc;
        sel_size = 0x208;
    } else {
        return false;
    }
    uint32_t idx = cmd - cmd_lo;
    if (idx >= sel_size) return false;
    uint8_t sel = 0;
    if (!safe_read_value(sel_va + idx, &sel)) return false;
    uint32_t thunk = 0;
    if (!safe_read_value(jmp_va + sel * 4, &thunk)) return false;
    // Thunk prologue: 8D 42 10  89 45 08  5D  E9 <rel32>
    uint8_t tb[12];
    if (!safe_read_bytes(thunk, tb, 12)) return false;
    if ((tb[0] != 0x8D) || (tb[1] != 0x42) || (tb[2] != 0x10)) return false;
    if (tb[7] != 0xE9) return false;
    int32_t rel = *(const int32_t *)(tb + 8);
    uint32_t stub = thunk + 12 + rel;
    // Stub prologue: 55 8B EC 53 56 8B B1 <disp32>
    // The first 5 bytes may have been replaced by our own jmp hook (E9 rel32) for
    // 641/648/651, but the `mov esi, [ecx+disp32]` opcode at offset 5 and its
    // disp32 at offset 7 are preserved — those are what we need. So we skip the
    // first-5-byte prologue check and only verify offsets 5, 6 and read disp at 7.
    uint8_t sb[11];
    if (!safe_read_bytes(stub, sb, 11)) {
        return false;
    }
    if ((sb[5] != 0x8B) || (sb[6] != 0xB1)) {
        return false;
    }
    uint32_t disp = *(const uint32_t *)(sb + 7);
    if (stub_va_out) *stub_va_out = stub;
    if (list_offset_out) *list_offset_out = disp;
    return true;
}

// --- Path B subscriber-diff cache ----------------------------------------------
// Remembers, per CMD, the node addresses seen last time this CMD was dispatched.
// On each dispatch we diff and emit [PathB.reg]/[PathB.unreg] events.
#define PATH_B_DIFF_CMDS 64
#define PATH_B_DIFF_NODES 16
struct PathBCmdState {
    uint32_t cmd_id;          // 0 = empty slot
    uint32_t nodes[PATH_B_DIFF_NODES];
    int node_count;
};
static PathBCmdState path_b_diff_state[PATH_B_DIFF_CMDS] = {};

static PathBCmdState *path_b_diff_slot(uint32_t cmd) {
    int free_slot = -1;
    for (int i = 0; i < PATH_B_DIFF_CMDS; i++) {
        if (path_b_diff_state[i].cmd_id == cmd) return &path_b_diff_state[i];
        if ((path_b_diff_state[i].cmd_id == 0) && (free_slot == -1)) free_slot = i;
    }
    if (free_slot >= 0) {
        path_b_diff_state[free_slot].cmd_id = cmd;
        path_b_diff_state[free_slot].node_count = 0;
        return &path_b_diff_state[free_slot];
    }
    return nullptr;
}

static void path_b_diff_update(uint32_t cmd, const uint32_t *nodes, int n) {
    PathBCmdState *s = path_b_diff_slot(cmd);
    if (s == nullptr) return;
    // New subscribers:
    for (int i = 0; i < n; i++) {
        bool was_there = false;
        for (int j = 0; j < s->node_count; j++) {
            if (s->nodes[j] == nodes[i]) { was_there = true; break; }
        }
        if (!was_there) {
            log("[PathB.reg] cmd=%u %s node=%p NEW since last dispatch\n",
                cmd, cmd_name(cmd), (void *)nodes[i]);
        }
    }
    // Removed subscribers:
    for (int j = 0; j < s->node_count; j++) {
        bool still_there = false;
        for (int i = 0; i < n; i++) {
            if (nodes[i] == s->nodes[j]) { still_there = true; break; }
        }
        if (!still_there) {
            log("[PathB.unreg] cmd=%u %s node=%p REMOVED since last dispatch\n",
                cmd, cmd_name(cmd), (void *)s->nodes[j]);
        }
    }
    // Update cache
    int nn = (n < PATH_B_DIFF_NODES) ? n : PATH_B_DIFF_NODES;
    s->node_count = nn;
    for (int i = 0; i < nn; i++) s->nodes[i] = nodes[i];
}

// Walks the subscriber list, logs current contents, and runs a diff against
// the last-seen set to emit registration/unregistration events.
static void path_b_walk_and_diff(const char *tag, uint32_t cgamelogic,
                                 uint32_t list_offset, uint32_t cmd_id) {
    uint32_t head = cgamelogic + list_offset;
    uint32_t first = 0;
    if (!safe_read_value(head, &first)) {
        log("[%s] cmd=%u %s this=%p head=%p head-unreadable\n",
            tag, cmd_id, cmd_name(cmd_id), (void *)cgamelogic, (void *)head);
        return;
    }
    uint32_t nodes[PATH_B_DIFF_NODES];
    int count = 0;
    if (first != head) {
        uint32_t node = first;
        while ((node != head) && (count < PATH_B_DIFF_NODES)) {
            nodes[count++] = node;
            uint32_t next = 0;
            if (!safe_read_value(node, &next)) break;
            node = next;
        }
    }
    if (count == 0) {
        log("[%s] cmd=%u %s this=%p head=%p subscribers:0\n",
            tag, cmd_id, cmd_name(cmd_id), (void *)cgamelogic, (void *)head);
    } else {
        log("[%s] cmd=%u %s this=%p head=%p subscribers=%d\n",
            tag, cmd_id, cmd_name(cmd_id), (void *)cgamelogic, (void *)head, count);
        for (int i = 0; i < count; i++) {
            uint32_t cb = 0;
            safe_read_value(nodes[i] + 12, &cb);
            DWORD off = relative_cry_offset(cb);
            if (off != 0) {
                log("  #%d node=%p cb=%p cry+0x%08X\n", i, (void *)nodes[i], (void *)cb, off);
            } else {
                log("  #%d node=%p cb=%p (non-crygame)\n", i, (void *)nodes[i], (void *)cb);
            }
        }
    }
    path_b_diff_update(cmd_id, nodes, count);
}

// CGameLogic::CmdDispatchSwitch @ crygame+0x65ede0 — Path B entry, giant switch(cmd_id).
// Each case tail-calls a Dispatch_CMD_<id> stub that fans out to a subscriber list.
// At hook entry, ecx = outer-this. The function immediately does `mov ecx, [ecx+0x3c]`
// to obtain the inner CGameLogic whose subscriber lists live at fixed offsets.
// Filtered to is_spawn_cmd() to avoid log flood.
//
// *** Automatic coverage ***: every CMD is resolved via `path_b_resolve` from the
// compact jump tables, so we walk + diff the subscriber list without needing a
// per-CMD hook or a manual table.
static void on_cmd_dispatch_switch(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t ret_addr = stack[0];
    uint32_t packet_ptr = stack[1];
    if (packet_ptr < 0x10000) {
        return;
    }
    uint16_t packet_id = 0;
    if (!safe_read_value(packet_ptr, &packet_id)) {
        return;
    }
    if (!is_spawn_cmd(packet_id)) {
        return;
    }
    uint32_t inner_this = 0;
    safe_read_value(ctx->ecx + 0x3c, &inner_this);
    uint32_t stub = 0;
    uint32_t list_off = 0;
    bool resolved = path_b_resolve(packet_id, &stub, &list_off);
    log("[CmdDispSwitch] outer=%p inner=%p cmd=%u %s packet=%p ret=%p cry+0x%08X stub=%p list_off:0x%x\n",
        (void *)ctx->ecx,
        (void *)inner_this,
        (unsigned)packet_id,
        cmd_name(packet_id),
        (void *)packet_ptr,
        (void *)ret_addr,
        relative_cry_offset(ret_addr),
        resolved ? (void *)stub : (void *)0,
        resolved ? list_off : 0u);
    if (resolved && (inner_this != 0)) {
        path_b_walk_and_diff("CmdDispSwitch.list", inner_this, list_off, packet_id);
    }
    // Caller-chain backtrace: walks the ebp frame pointer chain upstream to
    // identify the recv/parse layer that delivered this packet to Path B.
    log_backtrace(ctx, "CmdDispSwitch", 16);
}

// CGameLogic::Dispatch_CMD_641_MonsterLocomotion @ crygame+0x621040.
// Confirms the stub itself was reached (vs CmdDispSwitch fired but case defaulted).
// Uses the same walk-and-diff logic as on_cmd_dispatch_switch, sharing the diff
// cache so we don't log "new subscriber" twice for the same actual register event.
static void on_dispatch_cmd_641(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    log("[DispCmd641] enter this=%p ret=%p cry+0x%08X\n",
        (void *)ctx->ecx, (void *)stack[0], relative_cry_offset(stack[0]));
    path_b_walk_and_diff("DispCmd641", ctx->ecx, 0x570, 641);
}

static void on_dispatch_cmd_648(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    log("[DispCmd648] enter this=%p ret=%p cry+0x%08X\n",
        (void *)ctx->ecx, (void *)stack[0], relative_cry_offset(stack[0]));
    path_b_walk_and_diff("DispCmd648", ctx->ecx, 0x5e0, 648);
}

static void on_dispatch_cmd_651(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    log("[DispCmd651] enter this=%p ret=%p cry+0x%08X\n",
        (void *)ctx->ecx, (void *)stack[0], relative_cry_offset(stack[0]));
    path_b_walk_and_diff("DispCmd651", ctx->ecx, 0x5f8, 651);
}

// CBattleGround::BB_ReadSteeringProperties @ crygame+0xBA8B60 — Path C entry.
// Handles battle-scoped CMDs before eventually tail-calling CmdDispatchSwitch.
// At entry: ecx = this (CBattleGround), [ebp+8] = packet pointer.
// Signature: void __thiscall(this, packet_ptr)
static void on_bb_read_steering_properties(HookContext *ctx) {
    DWORD *stk = hook_stack(ctx);
    uint32_t packet_ptr = stk[1];
    uint16_t cmd_id = 0;
    if (packet_ptr >= 0x10000) {
        safe_read_value(packet_ptr, &cmd_id);
    }
    log("[BBReadSteering] this=%p packet=%p cmd=%u %s ret=%p cry+0x%08X tid=%lu\n",
        (void *)ctx->ecx, (void *)packet_ptr,
        (unsigned)cmd_id, cmd_name(cmd_id),
        (void *)stk[0], relative_cry_offset(stk[0]),
        GetCurrentThreadId());
}

// Path C-1 hook — the vt[5] call inside BB_ReadSteeringProperties at
// crygame+0xBA92CC. Intercepts 3 instructions: `mov eax,[ecx]; push ebx;
// call [eax+0x14]`. At hook entry:
//   ecx = sub-object pointer (= *(local_30+0x164)), loaded by prior mov
//   ebx = packet pointer
// We log the sub-object's vtable[5] target so we can classify what's at
// that method (TerSafe pass-through vs real dispatcher). After our callback
// and popad, the stolen bytes execute normally: reads vtable, pushes arg,
// calls vt[5]. Then jmp back to 0xBA92D2 continues the function.
static void on_path_c1_vt5_call(HookContext *ctx) {
    uint32_t sub_obj = ctx->ecx;
    uint32_t packet  = ctx->ebx;
    uint32_t vtable = 0, target = 0;
    if (sub_obj >= 0x10000) {
        if (safe_read_value(sub_obj, &vtable)) {
            if (vtable >= 0x10000) {
                safe_read_value(vtable + 0x14, &target);
            }
        }
    }
    DWORD *stk = hook_stack(ctx);
    log("[PathC1] sub_obj=%p vtable=%p vt[5]=%p cry+0x%08X packet=%p ret_after_call=%p tid=%lu\n",
        (void *)sub_obj, (void *)vtable,
        (void *)target, relative_cry_offset(target),
        (void *)packet,
        (void *)stk[0],
        GetCurrentThreadId());
}

static void on_call_handler(HookContext *ctx) {
    // CProtocolDispatch::OnMessage: [ebp+0xC] = packet body ptr, first uint16 = CMD ID
    uint8_t *packet_data = *(uint8_t **)(ctx->ebp + 0xC);
    if (packet_data == nullptr) {
        log("call_handler: packet_data NULL ecx=%p\n", (void *)ctx->ecx);
        return;
    }

    uint32_t packet_id = (uint32_t)(*(uint16_t *)packet_data);
    uint32_t field0 = 0;
    uint32_t field1 = 0;
    uint32_t field2 = 0;
    uint32_t field3 = 0;
    bool ok0 = false;
    bool ok1 = false;
    bool ok2 = false;
    bool ok3 = false;
    void *call_fn = nullptr;

    if (ctx->ecx != 0) {
        ok0 = safe_read_value(ctx->ecx + 0x00, &field0);
        ok1 = safe_read_value(ctx->ecx + 0x04, &field1);
        ok2 = safe_read_value(ctx->ecx + 0x08, &field2);
        ok3 = safe_read_value(ctx->ecx + 0x0C, &field3);
        if ((ok0) && (field0 > 0x10000)) {
            uint32_t vtable_slot0 = 0;
            if (safe_read_value(field0, &vtable_slot0)) {
                call_fn = (void *)vtable_slot0;
            }
        }
    }

    if (is_spawn_cmd(packet_id)) {
        log("call_handler_detail CMD=%u %s ecx=%p fields:[%p %p %p %p] ok:[%d %d %d %d] slot0=%p packet=%p\n",
            packet_id,
            cmd_name(packet_id),
            (void *)ctx->ecx,
            (void *)field0,
            (void *)field1,
            (void *)field2,
            (void *)field3,
            ok0 ? 1 : 0,
            ok1 ? 1 : 0,
            ok2 ? 1 : 0,
            ok3 ? 1 : 0,
            call_fn,
            packet_data);
    }

    /* Dump first 32 bytes of packet data for ALL handlers */
    {
        uint32_t *pd = (uint32_t *)packet_data;
        int dump_len = 32;
        log("call_handler CMD=%u %s def=%p vft=%p mgr=%p fn=%p unk=%u call_fn=%p packet=%p\n",
            packet_id, cmd_name(packet_id),
            (void *)field0, (void *)field1, (void *)field2, (void *)field3,
            ok3, call_fn, packet_data);
        if (!IsBadReadPtr(packet_data, dump_len)) {
            log("  pkt_hex: %08X %08X %08X %08X %08X %08X %08X %08X\n",
                pd[0], pd[1], pd[2], pd[3], pd[4], pd[5], pd[6], pd[7]);
        }
    }

    call_handler(call_fn, reinterpret_cast<HandlerCallbackDefintion *>(ctx->ecx), packet_id, packet_data);
}

static void on_monster_appear_single(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    int *param_4 = (int *)stack[3];
    int32_t count = -1;
    bool have_count = safe_read_value((uint32_t)param_4, &count);

    log("[CMD662] ENTER ret=%p session=%p cmd=%u param4=%p count=%d ok_count=%d\n",
        (void *)stack[0],
        (void *)stack[1],
        stack[2],
        param_4,
        count,
        have_count ? 1 : 0);
    log_game_logic_state("CMD662", ctx->ecx);
    log_runtime_globals("CMD662");

    if (have_count && (count > 0) && (count < 100)) {
        log_monster_appear_entry("CMD662", (const uint8_t *)(param_4 + 1), false);
    }
}

static void on_monster_appear_list(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    int *param_4 = (int *)stack[3];
    int32_t count = -1;
    bool have_count = safe_read_value((uint32_t)param_4, &count);

    log("[CMD663] ENTER ret=%p session=%p cmd=%u param4=%p count=%d ok_count=%d\n",
        (void *)stack[0],
        (void *)stack[1],
        stack[2],
        param_4,
        count,
        have_count ? 1 : 0);
    log_game_logic_state("CMD663", ctx->ecx);
    log_runtime_globals("CMD663");

    if (have_count && (count > 0) && (count < 100)) {
        log_monster_appear_entry("CMD663", (const uint8_t *)(param_4 + 1), false);
    }
}

static void on_controlled_monster_appear(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint8_t *param_4 = (uint8_t *)stack[3];

    log("[CMD714] ENTER ret=%p session=%p cmd=%u param4=%p\n",
        (void *)stack[0],
        (void *)stack[1],
        stack[2],
        param_4);
    log_game_logic_state("CMD714", ctx->ecx);
    log_runtime_globals("CMD714");
    if (param_4 != nullptr) {
        log_monster_appear_entry("CMD714", param_4, true);
    }
}

static void on_spawn_monsters(HookContext *ctx) {
    // SpawnMonsters is thiscall: ECX = this, stack = [ret, session, cmd_id, data_ptr]
    DWORD *stack = hook_stack(ctx);
    uint8_t *data = (uint8_t *)stack[3];
    uint32_t this_ptr = ctx->ecx;
    int32_t count = -1;
    if (data != nullptr) {
        count = read_unaligned<int32_t>(data);
    }

    log("[SpawnMonsters] ENTER ret=%p caller=%s session=%p cmd=%u data=%p count=%d\n",
        (void *)stack[0],
        classify_spawn_caller(stack[0]),
        (void *)stack[1],
        stack[2],
        data,
        count);
    log_game_logic_state("SpawnMonsters", this_ptr);
    log_runtime_globals("SpawnMonsters");

    if ((data != nullptr) && (count > 0) && (count < 100)) {
        log_monster_appear_entry("SpawnMonsters", data + 4, false);
    }
}

static void on_entity_join_wrapper(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t return_address = stack[0];
    uint32_t entity_ptr = stack[1];
    uint32_t info_ptr = 0;
    int32_t monster_type = -1;
    bool have_info = safe_read_value(entity_ptr + OFF_ENTITY_MONSTER_INFO, &info_ptr);
    bool have_type = (info_ptr >= 0x10000) && safe_read_value(info_ptr + OFF_MONSTER_INFO_TYPE, &monster_type);
    bool suspicious_type = (info_ptr >= 0x10000) && (!have_type || !monster_type_valid(monster_type));

    if (is_interesting_spawn_caller(return_address) || !have_info || info_ptr == 0 || suspicious_type) {
        log_entity_monster_info("EntityJoin", entity_ptr, return_address);
    }
}

static void on_set_monster_info(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t return_address = stack[0];
    uint32_t info_ptr = stack[1];
    log_set_monster_info("SetMonsterInfo", ctx->ecx, info_ptr, return_address);
}

static void on_get_monster_info(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t return_address = stack[0];
    if (!is_interesting_spawn_caller(return_address)) {
        return;
    }

    uint32_t entity_ptr = ctx->ecx;
    uint32_t info_ptr = 0;
    int32_t monster_type = -1;
    bool have_info = safe_read_value(entity_ptr + OFF_ENTITY_MONSTER_INFO, &info_ptr);
    bool have_type = (info_ptr >= 0x10000) && safe_read_value(info_ptr + OFF_MONSTER_INFO_TYPE, &monster_type);
    bool suspicious = !have_info || (info_ptr == 0) || ((info_ptr >= 0x10000) && (!have_type || !monster_type_valid(monster_type)));

    if (suspicious) {
        log_entity_monster_info("GetMonsterInfo", entity_ptr, return_address);
    }
}

static void on_init_from_appear(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t packet_ptr = stack[1];
    if (packet_ptr < 0x10000) {
        return;
    }

    uint32_t entity_net_id = 0;
    safe_read_value(ctx->ecx + 0x04, &entity_net_id);
    uint32_t packet_net_id = read_unaligned<uint32_t>(reinterpret_cast<const uint8_t *>(packet_ptr));
    if (!is_tracked_monster_net_id(entity_net_id) && !is_tracked_monster_net_id(packet_net_id)) {
        return;
    }

    log("[InitFromAppear] ret=%p entity=%p e+4=%u packet=%p packetNetId=%u\n",
        (void *)stack[0],
        (void *)ctx->ecx,
        entity_net_id,
        (void *)packet_ptr,
        packet_net_id);
    log_monster_appear_entry("InitFromAppear", reinterpret_cast<const uint8_t *>(packet_ptr), false);
}

// CBuffBase::SetPosition @ crygame+0x146B830. Vtable[8] of CCliLogicMonster (and
// many other entity types) — writes the 12-byte position vec3 to this+0x68/+0x6c/+0x70.
// We hook this to find who calls it for the active battle monster on each CMD 641.
//
// Filter: only log when `this` is a CCliLogicMonster instance (vtable 0x11d1f298).
// That keeps the noise down — players, projectiles, scene objects all share this
// SetPosition implementation through inheritance.
static void on_set_position(HookContext *ctx) {
    uint32_t this_ptr = (uint32_t)ctx->ecx;
    uint32_t vtable = 0;
    if (!safe_read_value(this_ptr, &vtable)) return;
    if (vtable != 0x11d1f298) return;  // only CCliLogicMonster (any NetID)

    uint32_t entity_net_id = 0;
    safe_read_value(this_ptr + 0x04, &entity_net_id);

    DWORD *stack = hook_stack(ctx);
    uint32_t ret_addr = stack[0];
    uint32_t pos_ptr = stack[1];
    float x = 0, y = 0, z = 0;
    safe_read_value(pos_ptr,     &x);
    safe_read_value(pos_ptr + 4, &y);
    safe_read_value(pos_ptr + 8, &z);

    char ret_buf[48];
    format_module_offset(ret_addr, ret_buf, sizeof(ret_buf));
    log("[SetPos] this=%p netid=%u pos=(%.3f, %.3f, %.3f) ret=%p (%s) tid=%lu\n",
        (void *)this_ptr, entity_net_id, x, y, z,
        (void *)ret_addr, ret_buf, GetCurrentThreadId());
}

static void on_btobj_simple_locomotion(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t packet_ptr = stack[1];
    if (packet_ptr < 0x10000) {
        return;
    }

    uint32_t entity_id = 0;
    if (!safe_read_value(packet_ptr + 0x04, &entity_id) || !is_tracked_monster_net_id(entity_id)) {
        return;
    }

    float pos_x = read_unaligned<float>(reinterpret_cast<const uint8_t *>(packet_ptr + 0x08));
    float pos_y = read_unaligned<float>(reinterpret_cast<const uint8_t *>(packet_ptr + 0x0C));
    float pos_z = read_unaligned<float>(reinterpret_cast<const uint8_t *>(packet_ptr + 0x10));
    float rot_x = read_unaligned<float>(reinterpret_cast<const uint8_t *>(packet_ptr + 0x14));
    float rot_y = read_unaligned<float>(reinterpret_cast<const uint8_t *>(packet_ptr + 0x18));
    float rot_z = read_unaligned<float>(reinterpret_cast<const uint8_t *>(packet_ptr + 0x1C));
    float rot_w = read_unaligned<float>(reinterpret_cast<const uint8_t *>(packet_ptr + 0x20));
    float target_x = read_unaligned<float>(reinterpret_cast<const uint8_t *>(packet_ptr + 0x24));
    float target_y = read_unaligned<float>(reinterpret_cast<const uint8_t *>(packet_ptr + 0x28));
    float target_z = read_unaligned<float>(reinterpret_cast<const uint8_t *>(packet_ptr + 0x2C));

    log("[CMD730] ENTER ret=%p ecx=%p packet=%p entity=%u pos=(%.2f,%.2f,%.2f) rot=(%.2f,%.2f,%.2f,%.2f) target=(%.2f,%.2f,%.2f)\n",
        (void *)stack[0],
        (void *)ctx->ecx,
        (void *)packet_ptr,
        entity_id,
        pos_x, pos_y, pos_z,
        rot_x, rot_y, rot_z, rot_w,
        target_x, target_y, target_z);
}

static void on_monster_ai_skill(HookContext *ctx) {
    uint32_t packet_ptr = 0;
    uint32_t return_address = 0;
    safe_read_value(ctx->ebp + 0x08, &packet_ptr);
    safe_read_value(ctx->ebp + 0x04, &return_address);
    if (packet_ptr < 0x10000) {
        return;
    }

    uint32_t entity_id = 0;
    uint32_t skill_id = 0;
    if (!safe_read_value(packet_ptr + 0x04, &entity_id) || !is_tracked_monster_net_id(entity_id)) {
        return;
    }
    safe_read_value(packet_ptr + 0x08, &skill_id);

    log("[CMD720] ENTER ret=%p packet=%p entity=%u skill=%u\n",
        (void *)return_address,
        (void *)packet_ptr,
        entity_id,
        skill_id);
}

static void on_log_notification(HookContext *ctx) {
    // Original asm read from [esp+0x28], [esp+0x2C], [esp+0x30]
    DWORD *stack = hook_stack(ctx);
    char *event_cstr = (char *)stack[2];
    InternalEventNotification *notif = (InternalEventNotification *)stack[3];
    uint32_t unk_group_index = stack[4];
    log_handle_notification(event_cstr, notif, unk_group_index);
}

// --- CStaticDataEnv init chain hooks ---

// Hook: FUN_11679c40 — CStaticDataEnv create + load from .dat files
// Called by FUN_10b7ffd0 (startup). cdecl: stack[1]=param_1(path), stack[2]=param_2, stack[3]=param_3(callback)
static void on_staticdata_init(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t param_1 = stack[1];
    uint32_t param_2 = stack[2];
    uint32_t param_3 = stack[3];
    uint32_t current_env = 0;
    safe_read_value(crygame_base + OFF_DAT_12387c6c, &current_env);
    log("[StaticDataEnv_Init] CALLED param1=%p param2=%p param3=%p current_env=%p\n",
        (void *)param_1, (void *)param_2, (void *)param_3, (void *)current_env);
    if (param_1 > 0x10000) {
        log("[StaticDataEnv_Init] param1_str: %.128s\n", (char *)param_1);
    }
}

// Hook: FUN_11679cc0 — CStaticDataEnv direct setter (called by level load + FUN_11183960)
// cdecl: stack[1]=param_1 (CStaticDataEnv pointer to set)
static void on_staticdata_set(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t value = stack[1];
    uint32_t current_env = 0;
    safe_read_value(crygame_base + OFF_DAT_12387c6c, &current_env);
    log("[StaticDataEnv_Set] CALLED value=%p current_env=%p\n",
        (void *)value, (void *)current_env);
}

// Hook: FUN_10c3f520 — Level load function (initializes CStaticDataEnv AND BattleGround)
// thiscall: ECX=this, stack[1]=param_2, stack[2]=param_3
// param_3+0xa70 has pointer-to-pointer to CStaticDataEnv
static void on_level_load(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t this_ptr = ctx->ecx;
    uint32_t param_2 = stack[1];
    uint32_t param_3 = stack[2];
    uint32_t current_env = 0;
    uint32_t bg = 0;
    uint32_t gr = 0;
    safe_read_value(crygame_base + OFF_DAT_12387c6c, &current_env);
    safe_read_value(crygame_base + OFF_g_pCBattleGround, &bg);
    safe_read_value(crygame_base + OFF_g_pCGameRules, &gr);
    log("[LevelLoad] CALLED this=%p param2=%p param3=%p env=%p BG=%p GR=%p\n",
        (void *)this_ptr, (void *)param_2, (void *)param_3,
        (void *)current_env, (void *)bg, (void *)gr);
    if (param_3 > 0x10000) {
        uint32_t ptr_at_a70 = 0;
        if (safe_read_value(param_3 + 0xa70, &ptr_at_a70)) {
            log("[LevelLoad] param3+0xa70=%p\n", (void *)ptr_at_a70);
            if (ptr_at_a70 > 0x10000) {
                uint32_t env_val = 0;
                if (safe_read_value(ptr_at_a70, &env_val)) {
                    log("[LevelLoad] *(param3+0xa70)=%p (CStaticDataEnv candidate)\n", (void *)env_val);
                    if (env_val > 0x10000) {
                        uint8_t loaded_flag = 0;
                        if (safe_read_value(env_val, &loaded_flag)) {
                            log("[LevelLoad] env loaded_flag=%u\n", loaded_flag);
                        }
                    }
                }
            }
        }
    }
}

// Hook: FUN_10b7ffd0 — Startup init (creates CStaticDataEnv from current dir .dat files)
// thiscall: ECX=this, stack[1]=param_2(char* path), stack[2]=param_3, stack[3]=param_4
static void on_startup_init(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t this_ptr = ctx->ecx;
    uint32_t param_2 = stack[1];
    uint32_t param_3 = stack[2];
    uint32_t param_4 = stack[3];
    log("[StartupInit] CALLED this=%p param2=%p param3=%p param4=%p\n",
        (void *)this_ptr, (void *)param_2, (void *)param_3, (void *)param_4);
    if (param_2 > 0x10000) {
        log("[StartupInit] path: %.128s\n", (char *)param_2);
    }
}

// Hook: FUN_1169ed30 — CStaticDataEnv LoadData (actually loads .dat files)
// thiscall: ECX=this(CStaticDataEnv*), stack[1]=param_2(path/descriptor), stack[2]=param_3(name), stack[3]=param_4
static void on_load_data(HookContext *ctx) {
    DWORD *stack = hook_stack(ctx);
    uint32_t this_ptr = ctx->ecx;
    uint32_t param_2 = stack[1];
    uint32_t param_3 = stack[2];
    uint32_t param_4 = stack[3];
    log("[LoadData] CALLED this(env):%p param2=%p param3=%p param4=%p\n",
        (void *)this_ptr, (void *)param_2, (void *)param_3, (void *)param_4);
    if (param_3 > 0x10000) {
        log("[LoadData] param3_str: %.128s\n", (char *)param_3);
    }
}

// --- stolen bytes ---

// crygame + 0x1223631: mov ebp,esp (8B EC) + mov eax,[ecx+0Ch] (8B 41 0C) = 5 bytes
static const uint8_t register_handler_stolen[] = {0x8B, 0xEC, 0x8B, 0x41, 0x0C};

// crygame + 0x12408f0: push ebp + mov ebp,esp + push esi + mov esi,[ecx+04] = 7 bytes
static const uint8_t protocol_on_recv_msg_stolen[] = {0x55, 0x8B, 0xEC, 0x56, 0x8B, 0x71, 0x04};

// crygame + 0x12230c0: push ebp + mov ebp,esp + mov eax,[ebp+0Ch] + push ebx + push esi + push edi = 9 bytes
static const uint8_t protocol_dispatch_entry_stolen[] = {0x55, 0x8B, 0xEC, 0x8B, 0x45, 0x0C, 0x53, 0x56, 0x57};

// crygame + 0x1223148: push [ebp+0Ch] (FF 75 0C) + mov eax,[ecx] (8B 01) = 5 bytes
static const uint8_t call_handler_stolen[] = {0xFF, 0x75, 0x0C, 0x8B, 0x01};

// crygame + 0x146b830 CBuffBase::SetPosition prologue:
//   push ebp; mov ebp,esp; mov edx,[ebp+8] = 6 bytes
static const uint8_t set_position_stolen[] = {0x55, 0x8B, 0xEC, 0x8B, 0x55, 0x08};

// crygame + 0x12a5580: push ebp + mov ebp,esp + sub esp,1C + push edi + mov edi,ecx = 9 bytes
static const uint8_t monster_appear_single_stolen[] = {0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x1C, 0x57, 0x8B, 0xF9};

// crygame + 0x12a3ac0: push ebp + mov ebp,esp + sub esp,10 + push ebx + push edi + mov edi,ecx = 10 bytes
static const uint8_t monster_appear_list_stolen[] = {0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x10, 0x53, 0x57, 0x8B, 0xF9};

// crygame + 0x129f8b0: push ebp + mov ebp,esp + sub esp,44 + push ebx + push esi + mov esi,ecx = 10 bytes
static const uint8_t controlled_monster_appear_stolen[] = {0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x44, 0x53, 0x56, 0x8B, 0xF1};

// crygame + 0x12a2e10: push ebp + mov ebp,esp + sub esp,1Ch = 6 bytes
static const uint8_t spawn_monsters_stolen[] = {0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x1C};

// crygame + 0x12ad180: push ebp + mov ebp,esp + push [ebp+08] = 6 bytes
static const uint8_t entity_join_wrapper_stolen[] = {0x55, 0x8B, 0xEC, 0xFF, 0x75, 0x08};

// crygame + 0x13a3da0: push ebp + mov ebp,esp + mov eax,[ebp+08] = 6 bytes
static const uint8_t set_monster_info_stolen[] = {0x55, 0x8B, 0xEC, 0x8B, 0x45, 0x08};

// crygame + 0x13a23b0: mov eax,[ecx+1250] + test eax,eax = 8 bytes
static const uint8_t get_monster_info_stolen[] = {0x8B, 0x81, 0x50, 0x12, 0x00, 0x00, 0x85, 0xC0};

// crygame + 0x13a3720: push ebp + mov ebp,esp + sub esp,10 + push ebx + mov ebx,[ebp+08] + push esi + push edi + mov edi,ecx = 14 bytes
static const uint8_t init_from_appear_stolen[] = {0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x10, 0x53, 0x8B, 0x5D, 0x08, 0x56, 0x57, 0x8B, 0xF9};

// crygame + 0x1289a00: push ebp + mov ebp,esp + mov eax,[ecx] + sub esp,2C = 8 bytes
static const uint8_t btobj_simple_locomotion_stolen[] = {0x55, 0x8B, 0xEC, 0x8B, 0x01, 0x83, 0xEC, 0x2C};

// crygame + 0x12e3bec: mov ecx,[ebp+08] + mov edx,[eax] + push [ecx+04] = 7 bytes
static const uint8_t monster_ai_skill_stolen[] = {0x8B, 0x4D, 0x08, 0x8B, 0x10, 0xFF, 0x71, 0x04};

// crygame + 0x50ab31: mov ebp,esp (8B EC) + sub esp,18h (83 EC 18) = 5 bytes
static const uint8_t log_notification_stolen[] = {0x8B, 0xEC, 0x83, 0xEC, 0x18};

// crygame + 0x1679c40: push ebp(55) + mov ebp,esp(8BEC) + push esi(56) + mov esi,[ebp+10](8B7510) = 7 bytes
static const uint8_t staticdata_init_stolen[] = {0x55, 0x8B, 0xEC, 0x56, 0x8B, 0x75, 0x10};

// crygame + 0x1679cc0: push ebp(55) + mov ebp,esp(8BEC) + cmp [12387c6c],0 (833D6C7C381200) = 10 bytes
static const uint8_t staticdata_set_stolen[] = {
    0x55, 0x8B, 0xEC, 0x83, 0x3D, 0x6C, 0x7C, 0x38, 0x12, 0x00
};

// crygame + 0xc3f520: push ebp(55) + mov ebp,esp(8BEC) + push ecx(51) + push ebx(53) = 5 bytes
static const uint8_t level_load_stolen[] = {0x55, 0x8B, 0xEC, 0x51, 0x53};

// crygame + 0xb7ffd0: push ebp(55) + mov ebp,esp(8BEC) + sub esp,0xBE0(81ECE00B0000) = 9 bytes
static const uint8_t startup_init_stolen[] = {
    0x55, 0x8B, 0xEC, 0x81, 0xEC, 0xE0, 0x0B, 0x00, 0x00
};

// crygame + 0x169ed30: push ebp(55) + mov ebp,esp(8BEC) + push esi(56) + mov esi,ecx(8BF1) = 6 bytes
static const uint8_t load_data_stolen[] = {0x55, 0x8B, 0xEC, 0x56, 0x8B, 0xF1};

// --- Send function hook (FUN_10459870) — logs call stack to trace TerSafe send path ---
// 0x459870: thiscall, ECX = stream context, [ebp+8] = buffer descriptor ptr
// stolen: push ebp(55) + mov ebp,esp(8BEC) + push ecx(51) + push ebx(53) + push esi(56) = 6 bytes
// --- Pre-dispatch function hook (called by VM wrapper before co_vt[2]) ---
// 0xF0955D: Called with args on stack: (cmd, ebx_val, &local, something)
// Returns non-zero if packet should continue to dispatch
static void on_pre_dispatch(HookContext *ctx) {
    DWORD *stk = hook_stack(ctx);
    DWORD ret_addr = stk[0];
    DWORD arg1 = stk[1];  // cmd?
    DWORD arg2 = stk[2];  //
    DWORD arg3 = stk[3];  // &local?
    DWORD arg4 = stk[4];  // something
    log("[pre_dispatch] caller=0x%X args=[0x%X 0x%X 0x%X 0x%X] ecx=0x%X edx=0x%X\n",
        ret_addr, arg1, arg2, arg3, arg4, ctx->ecx, ctx->edx);
}
static const uint8_t pre_dispatch_stolen[] = {0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x1C};

// --- TerSafe proxy observation hooks ---
// CTerSafeProxy stores CreateObj ptr at this+0x10034C, state at this+0x100350

// 0x45DA50: co_vt[1] wrapper — calls createObj->vtable[1](dataBuffer)
// thiscall: ECX = CTerSafeProxy this
// stolen: mov eax,ecx(8BC1) + mov ecx,[eax+0x10034C](8B884C031000) = 8 bytes
static void on_tersafe_vt1_wrapper(HookContext *ctx) {
    DWORD self = ctx->ecx;
    DWORD obj = 0, state = 0;
    if (self) {
        obj = *(DWORD *)(self + 0x10034C);
        state = *(DWORD *)(self + 0x100350);
    }
    log("[ts:vt1_wrap] this=%p obj=%p state=%d\n", (void*)self, (void*)obj, state);
}
static const uint8_t tersafe_vt1_stolen[] = {0x8B, 0xC1, 0x8B, 0x88, 0x4C, 0x03, 0x10, 0x00};

// 0x45DBC0: co_vt[3] timing wrapper — calls createObj->vtable[3](queryParam)
// thiscall: ECX = CTerSafeProxy, [ebp+8] = queryParam, [ebp+0xC] = outTimeDelta*
// stolen: push ebp(55) + mov ebp,esp(8BEC) + sub esp,0x10(83EC10) = 6 bytes
static void on_tersafe_vt3_wrapper(HookContext *ctx) {
    DWORD self = ctx->ecx;
    DWORD *stk = hook_stack(ctx);
    DWORD ret_addr = stk[0];
    DWORD queryParam = stk[1];
    DWORD outPtr = stk[2];
    DWORD obj = 0, state = 0;
    if (self) {
        obj = *(DWORD *)(self + 0x10034C);
        state = *(DWORD *)(self + 0x100350);
    }
    log("[ts:vt3_wrap] this=%p obj=%p state=%d param=0x%X outPtr=%p caller=0x%X\n",
        (void*)self, (void*)obj, state, queryParam, (void*)outPtr, ret_addr);
}
static const uint8_t tersafe_vt3_stolen[] = {0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x10};

// 0x45A090: slot30 — packet processor, calls VM wrapper → co_vt[2]
// thiscall: ECX = CTerSafeProxy, [ebp+8] = packet*
// stolen: push ebp(55) + mov ebp,esp(8BEC) + and esp,-8(83E4F8) = 6 bytes
static void on_tersafe_slot30(HookContext *ctx) {
    DWORD self = ctx->ecx;
    DWORD *stk = hook_stack(ctx);
    DWORD ret_addr = stk[0];
    DWORD pkt = stk[1];
    DWORD conn = 0;
    if (self && !IsBadReadPtr((void*)(self + 0xC), 4)) {
        conn = *(DWORD *)(self + 0xC);
    }
    uint16_t cmd = 0;
    if (pkt && !IsBadReadPtr((void*)pkt, 2)) {
        cmd = *(uint16_t *)pkt;
    }
    log("[ts:slot30] this=%p conn=%p pkt=%p cmd=0x%X caller=0x%X\n",
        (void*)self, (void*)conn, (void*)pkt, cmd, ret_addr);

    /* One-shot dump of CTerSafeProxy+0x90..+0xDF to see the +0xAC region */
    static int dumped_ac = 0;
    if (!dumped_ac && cmd == 0x504 && self && !IsBadReadPtr((void*)(self + 0x90), 0x50)) {
        dumped_ac = 1;
        uint32_t *d = (uint32_t *)(self + 0x90);
        log("[ts:proxy +0x90..+0xDF]");
        for (int i = 0; i < 20; i++) {
            if ((i % 8) == 0) log("\n  +0x%02X:", 0x90 + i * 4);
            log(" %08X", d[i]);
        }
        log("\n");
    }
}
static const uint8_t tersafe_slot30_stolen[] = {0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8};

// crygame + 0x621040 (Dispatch_CMD_641_MonsterLocomotion) prologue:
//   push ebp(55) + mov ebp,esp(8B EC) + push ebx(53) + push esi(56) = 5 bytes
// Same prologue shape for 0x621660 (CMD 648) and 0x6217b0 (CMD 651).
static const uint8_t dispatch_cmd_641_stolen[] = {0x55, 0x8B, 0xEC, 0x53, 0x56};
static const uint8_t dispatch_cmd_648_stolen[] = {0x55, 0x8B, 0xEC, 0x53, 0x56};
static const uint8_t dispatch_cmd_651_stolen[] = {0x55, 0x8B, 0xEC, 0x53, 0x56};

// crygame + 0x65ede0 (CmdDispatchSwitch) prologue:
//   push ebp(55) + mov ebp,esp(8B EC) + mov ecx,[ecx+0x3c](8B 49 3C) = 6 bytes
static const uint8_t cmd_dispatch_switch_stolen[] = {0x55, 0x8B, 0xEC, 0x8B, 0x49, 0x3C};

// crygame + 0xBA8B60 (BB_ReadSteeringProperties) prologue:
//   push ebp(55) + mov ebp,esp(8B EC) + sub esp,0x2A0(81 EC A0 02 00 00) = 9 bytes
static const uint8_t bb_read_steering_stolen[] = {
    0x55, 0x8B, 0xEC, 0x81, 0xEC, 0xA0, 0x02, 0x00, 0x00
};

// crygame + 0xBA92CC — mid-function hook at the vt[5] call site inside
// BB_ReadSteeringProperties. Captures: mov eax,[ecx](8B 01) + push ebx(53)
// + call [eax+0x14](FF 50 14) = 6 bytes. After the trampoline runs these
// stolen bytes, control resumes at 0xBA92D2 (past the original call site).
static const uint8_t path_c1_vt5_stolen[] = {0x8B, 0x01, 0x53, 0xFF, 0x50, 0x14};

// --- TerSafe proxy observation hooks ---
// Opt-in via ag_mho.ini `hook_tersafe = 1`. Disabled by default because they
// fire on every packet and are very noisy; useful only for TenProtect/TerSafe
// investigation.
static const JmpHookEntry crygame_tersafe_jmp_hooks[] = {
    {0x45DA50,   on_tersafe_vt1_wrapper,       tersafe_vt1_stolen,               sizeof(tersafe_vt1_stolen)},
    {0x45DBC0,   on_tersafe_vt3_wrapper,       tersafe_vt3_stolen,               sizeof(tersafe_vt3_stolen)},
    {0x45A090,   on_tersafe_slot30,            tersafe_slot30_stolen,            sizeof(tersafe_slot30_stolen)},
    {0xF0955D,   on_pre_dispatch,              pre_dispatch_stolen,              sizeof(pre_dispatch_stolen)},
};

static const JmpHookEntry crygame_jmp_hooks[] = {
    {0x12408f0,  on_protocol_handler_recv_msg, protocol_on_recv_msg_stolen,     sizeof(protocol_on_recv_msg_stolen)},
    {0x12230c0,  on_protocol_dispatch_entry,   protocol_dispatch_entry_stolen,   sizeof(protocol_dispatch_entry_stolen)},
    {0x1223631,  on_register_handler,          register_handler_stolen,          sizeof(register_handler_stolen)},
    {0x1223148,  on_call_handler,              call_handler_stolen,              sizeof(call_handler_stolen)},
    {0x12a5580,  on_monster_appear_single,     monster_appear_single_stolen,     sizeof(monster_appear_single_stolen)},
    {0x12a3ac0,  on_monster_appear_list,       monster_appear_list_stolen,       sizeof(monster_appear_list_stolen)},
    {0x129f8b0,  on_controlled_monster_appear, controlled_monster_appear_stolen, sizeof(controlled_monster_appear_stolen)},
    {0x12a2e10,  on_spawn_monsters,            spawn_monsters_stolen,            sizeof(spawn_monsters_stolen)},
    {0x12ad180,  on_entity_join_wrapper,       entity_join_wrapper_stolen,       sizeof(entity_join_wrapper_stolen)},
    {0x13a3da0,  on_set_monster_info,          set_monster_info_stolen,          sizeof(set_monster_info_stolen)},
    {0x13a23b0,  on_get_monster_info,          get_monster_info_stolen,          sizeof(get_monster_info_stolen)},
    {0x13a3720,  on_init_from_appear,          init_from_appear_stolen,          sizeof(init_from_appear_stolen)},
    {0x146b830,  on_set_position,              set_position_stolen,              sizeof(set_position_stolen)},
    {0x1289a00,  on_btobj_simple_locomotion,   btobj_simple_locomotion_stolen,   sizeof(btobj_simple_locomotion_stolen)},
    {0x12e3bec,  on_monster_ai_skill,          monster_ai_skill_stolen,          sizeof(monster_ai_skill_stolen)},
    {0x50ab31,   on_log_notification,          log_notification_stolen,          sizeof(log_notification_stolen)},
    {0x1679c40,  on_staticdata_init,           staticdata_init_stolen,           sizeof(staticdata_init_stolen)},
    {0x1679cc0,  on_staticdata_set,            staticdata_set_stolen,            sizeof(staticdata_set_stolen)},
    {0x0c3f520,  on_level_load,                level_load_stolen,                sizeof(level_load_stolen)},
    {0x0b7ffd0,  on_startup_init,              startup_init_stolen,              sizeof(startup_init_stolen)},
    {0x169ed30,  on_load_data,                 load_data_stolen,                 sizeof(load_data_stolen)},
    {0x65ede0,   on_cmd_dispatch_switch,       cmd_dispatch_switch_stolen,       sizeof(cmd_dispatch_switch_stolen)},
    {0x621040,   on_dispatch_cmd_641,          dispatch_cmd_641_stolen,          sizeof(dispatch_cmd_641_stolen)},
    {0x621660,   on_dispatch_cmd_648,          dispatch_cmd_648_stolen,          sizeof(dispatch_cmd_648_stolen)},
    {0x6217b0,   on_dispatch_cmd_651,          dispatch_cmd_651_stolen,          sizeof(dispatch_cmd_651_stolen)},
    {0xBA8B60,   on_bb_read_steering_properties, bb_read_steering_stolen,        sizeof(bb_read_steering_stolen)},
    {0xBA92CC,   on_path_c1_vt5_call,          path_c1_vt5_stolen,               sizeof(path_c1_vt5_stolen)},
};

// --- install ---

void install_crygame_hooks() {
    DWORD base = wait_for_module("crygame");
    crygame_base = base;
    log("got crygame: %p \n", (void *)base);

    // Resolve other module bases for the backtrace formatter. These may be 0
    // if the module isn't loaded yet — backtrace just prints "?+0x…" then.
    mhoclient_base_bt = (DWORD)GetModuleHandleA("MHOClient.exe");
    tenproxy_base_bt  = (DWORD)GetModuleHandleA("tenproxy.dll");
    protocal_base_bt  = (DWORD)GetModuleHandleA("protocalhandler.dll");
    msvcr120_base_bt  = (DWORD)GetModuleHandleA("MSVCR120.dll");
    ntdll_base_bt     = (DWORD)GetModuleHandleA("ntdll.dll");
    kernel32_base_bt  = (DWORD)GetModuleHandleA("kernel32.dll");
    log("bt modules: mho=%p tp=%p ph=%p msvcr=%p ntdll=%p k32=%p\n",
        (void *)mhoclient_base_bt, (void *)tenproxy_base_bt,
        (void *)protocal_base_bt, (void *)msvcr120_base_bt,
        (void *)ntdll_base_bt, (void *)kernel32_base_bt);

    // Log initial state of CStaticDataEnv
    uint32_t env0 = 0;
    uint32_t flag0 = 0;
    safe_read_value(base + OFF_DAT_12387c6c, &env0);
    safe_read_value(base + OFF_DAT_12387c70, &flag0);
    log("[init] CStaticDataEnv=%p flag=%u\n", (void *)env0, flag0);

    // Check if CStaticDataEnv was loaded (first byte = loaded flag)
    if (env0 > 0x10000) {
        uint8_t loaded_flag = 0;
        if (safe_read_value(env0, &loaded_flag)) {
            log("[init] CStaticDataEnv->loaded_flag=%u\n", loaded_flag);
        }
    }

    // Check if CGame::Init (FUN_10b7ffd0) prologue is still intact
    // Original bytes: 55 8B EC 81 EC E0 0B 00 00
    uint8_t cgame_init[5] = {};
    if (safe_read_bytes(base + 0x0b7ffd0, cgame_init, sizeof(cgame_init))) {
        log("[init] CGame::Init bytes: %02x %02x %02x %02x %02x\n",
            cgame_init[0], cgame_init[1], cgame_init[2], cgame_init[3], cgame_init[4]);
    }

    // Check g_CMonsterInfo_SingletonPtr
    uint32_t moninfo0 = 0;
    safe_read_value(base + OFF_g_CMonsterInfo_Ptr, &moninfo0);
    log("[init] g_CMonsterInfo_SingletonPtr=%p\n", (void *)moninfo0);

    // Dump CStaticDataEnv internal structure (first 32 bytes)
    if (env0 > 0x10000) {
        uint32_t env_data[8] = {};
        if (safe_read_bytes(env0, env_data, sizeof(env_data))) {
            log("[init] CStaticDataEnv dump: %08x %08x %08x %08x %08x %08x %08x %08x\n",
                env_data[0], env_data[1], env_data[2], env_data[3],
                env_data[4], env_data[5], env_data[6], env_data[7]);
        }
    }

    org_fn_crygame_13EC290 = (fn_crygame_13EC290)(base + 0x13F3640);

    // ini is created at DLL startup in dllmain `run()` before any hook thread.
    std::wstring ini_path = get_exe_dir() + L"ag_mho.ini";
    auto ag_cfg = ag_ini_read(ini_path);
    bool enable_tersafe = ag_ini_get_int(ag_cfg, "hook_tersafe", 0) != 0;
    log("config hook_tersafe = %d %s\n",
        enable_tersafe ? 1 : 0,
        enable_tersafe ? "(tersafe hooks installed)" : "(tersafe hooks skipped)");

    install_jmp_hooks(base, crygame_jmp_hooks, std::size(crygame_jmp_hooks));
    if (enable_tersafe) {
        install_jmp_hooks(base, crygame_tersafe_jmp_hooks, std::size(crygame_tersafe_jmp_hooks));
    }
}
