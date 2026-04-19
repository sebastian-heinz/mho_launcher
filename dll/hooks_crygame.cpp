#include "hooks_crygame.h"
#include "hooks_mhoclient.h"  // server_url_address
#include "log.h"
#include "hook_table.h"

#include "mho_types.h"
#include "cs_cmd.h"
#include "internal_notification_ids.h"

#include <cstdint>
#include <cstring>
#include <string>

// CryGame.dll base address — set during install_crygame_hooks
static DWORD crygame_base = 0;

// Key global offsets (relative to CryGame.dll base = 0x10000000)
static constexpr DWORD OFF_DAT_12387c6c         = 0x2387c6c;  // CStaticDataEnv pointer
static constexpr DWORD OFF_DAT_12387c70         = 0x2387c70;  // CStaticDataEnv init flag
static constexpr DWORD OFF_g_CMonsterInfo_Ptr   = 0x23bee60;  // g_CMonsterInfo_SingletonPtr
static constexpr DWORD OFF_g_pCGameRules        = 0x20286b8;
static constexpr DWORD OFF_g_pCBattleGround     = 0x20286c8;

// --- original function pointers ---

static fn_crygame_13EC290 org_fn_crygame_13EC290 = nullptr;

// --- handler callbacks (called from hook context wrappers below) ---

static void __cdecl crygame_13F3640() {
    log("crygame_13F3640\n");
    const char *url = "127.0.0.1:8142";
    WriteMemory((LPVOID) server_url_address, url, strlen(url));
    org_fn_crygame_13EC290();
}

static void register_handler(uint32_t packet_id, HandlerCallbackDefintion *handler_definition) {
    CsCmd cmd;
    bool found = false;
    for (int i = 0; i < CMDS_SIZE; i++) {
        if (CMDS[i].id == packet_id) {
            found = true;
            cmd = CMDS[i];
            break;
        }
    }
    if (found) {
        log("register_handler %u %s handler_fn:%p, unk:%u\n",
            packet_id, cmd.name.c_str(),
            handler_definition->handler_callback_function_ptr,
            handler_definition->unknown_field);
    } else {
        log("register_handler !!! NOT FOUND !!! PacketId: %u \n", packet_id);
    }
}

static void call_handler(void *call_fn, HandlerCallbackDefintion *handler_definition,
                          uint32_t packet_id, uint8_t *packet_data) {
    CsCmd cmd;
    bool found = false;
    for (int i = 0; i < CMDS_SIZE; i++) {
        if (CMDS[i].id == packet_id) {
            found = true;
            cmd = CMDS[i];
            break;
        }
    }
    if (found) {
        if (handler_definition) {
            log("call_handler: CMD:%u %s handler_fn:%p, unk:%u, call_fn:%p\n",
                packet_id, cmd.name.c_str(),
                handler_definition->handler_callback_function_ptr,
                handler_definition->unknown_field, call_fn);
        } else {
            log("call_handler: CMD:%u %s call_fn:%p\n", packet_id, cmd.name.c_str(), call_fn);
        }
    } else {
        log("call_handler: !!! NOT FOUND !!! CMD:%u call_fn:%p\n", packet_id, call_fn);
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
    log("log_handle_logic_or_game_event_notification(event:%s, notification_id:%u (type: %s), unk_group_idx: %u)\n",
        event_cstr, notif->notification_id, notification_type.c_str(), unk_group_index);
}

// --- hook context callbacks ---

static void on_register_handler(HookContext *ctx) {
    // Original asm read from [esp+0x28] and [esp+0x2C] (= stack args at +0x8 and +0xC past pushad)
    DWORD *stack = hook_stack(ctx);
    uint32_t packet_id = stack[2];                                      // [original_esp + 0x8]
    HandlerCallbackDefintion *handler_def = (HandlerCallbackDefintion *)stack[3];  // [original_esp + 0xC]
    register_handler(packet_id, handler_def);
}

static void on_call_handler(HookContext *ctx) {
    // CProtocolDispatch::OnMessage: [ebp+0xC] = packet body ptr, first uint16 = CMD ID
    uint8_t *packet_data = *(uint8_t **)(ctx->ebp + 0xC);
    uint32_t packet_id = (uint32_t)(*(uint16_t *)packet_data);
    void *call_fn = nullptr;
    if (ctx->ecx) {
        uint32_t vtable = *(uint32_t *)(ctx->ecx);
        if (vtable > 0x10000) {
            call_fn = *(void **)vtable;
        } else {
            log("call_handler: CMD:%u ecx:%p vtable:%p INVALID\n", packet_id, (void *)ctx->ecx, (void *)vtable);
        }
    } else {
        log("call_handler: CMD:%u ecx:NULL\n", packet_id);
    }

    call_handler(call_fn, nullptr, packet_id, packet_data);
}

static void on_spawn_monsters(HookContext *ctx) {
    // SpawnMonsters is thiscall: ECX = this, stack = [ret, session, cmd_id, data_ptr]
    DWORD *stack = hook_stack(ctx);
    uint8_t *data = (uint8_t *)stack[3];
    uint32_t this_ptr = ctx->ecx;
    uint32_t game_rules = *(uint32_t *)(this_ptr + 0xc);
    int32_t count = *(int32_t *)data;

    // Log CStaticDataEnv and CMonsterInfo state — the critical init chain
    uint32_t static_env = *(uint32_t *)(crygame_base + OFF_DAT_12387c6c);
    uint32_t env_flag   = *(uint32_t *)(crygame_base + OFF_DAT_12387c70);
    uint32_t moninfo_ptr = *(uint32_t *)(crygame_base + OFF_g_CMonsterInfo_Ptr);
    uint32_t bg_ptr     = *(uint32_t *)(crygame_base + OFF_g_pCBattleGround);
    uint32_t gr_ptr     = *(uint32_t *)(crygame_base + OFF_g_pCGameRules);
    uint32_t gr_78      = (gr_ptr > 0x10000) ? *(uint32_t *)(gr_ptr + 0x78) : 0;

    log("[SpawnMonsters] this:%p +0xc(gameRules):%p count:%d\n",
        (void *)this_ptr, (void *)game_rules, count);
    log("[SpawnMonsters] CStaticDataEnv:%p flag:%u CMonsterInfo:%p BG:%p GR:%p GR+78:%p\n",
        (void *)static_env, env_flag, (void *)moninfo_ptr,
        (void *)bg_ptr, (void *)gr_ptr, (void *)gr_78);

    if (count > 0 && count < 100) {
        uint8_t *entry = data + 4;
        uint32_t netId = *(uint32_t *)entry;
        int16_t spawnType = *(int16_t *)(entry + 4);
        int32_t monInfoId = *(int32_t *)(entry + 6);
        log("[SpawnMonsters] NetId:%u SpawnType:%d MonsterInfoId:%d\n",
            netId, spawnType, monInfoId);
        log("[SpawnMonsters] data: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],
            data[8],data[9],data[10],data[11],data[12],data[13],data[14],data[15]);
    }
}

static void on_log_notification(HookContext *ctx) {
    // Original asm read from [esp+0x28], [esp+0x2C], [esp+0x30]
    DWORD *stack = hook_stack(ctx);
    char *event_cstr = (char *)stack[2];                                // [original_esp + 0x8]
    InternalEventNotification *notif = (InternalEventNotification *)stack[3];  // [original_esp + 0xC]
    uint32_t unk_group_index = stack[4];                                // [original_esp + 0x10]
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
    uint32_t current_env = *(uint32_t *)(crygame_base + OFF_DAT_12387c6c);
    log("[StaticDataEnv_Init] CALLED param1:%p param2:%p param3:%p current_env:%p\n",
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
    uint32_t current_env = *(uint32_t *)(crygame_base + OFF_DAT_12387c6c);
    log("[StaticDataEnv_Set] CALLED value:%p current_env:%p\n",
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
    uint32_t current_env = *(uint32_t *)(crygame_base + OFF_DAT_12387c6c);
    uint32_t bg = *(uint32_t *)(crygame_base + OFF_g_pCBattleGround);
    uint32_t gr = *(uint32_t *)(crygame_base + OFF_g_pCGameRules);
    log("[LevelLoad] CALLED this:%p param2:%p param3:%p env:%p BG:%p GR:%p\n",
        (void *)this_ptr, (void *)param_2, (void *)param_3,
        (void *)current_env, (void *)bg, (void *)gr);
    if (param_3 > 0x10000) {
        uint32_t ptr_at_a70 = *(uint32_t *)(param_3 + 0xa70);
        log("[LevelLoad] param3+0xa70=%p\n", (void *)ptr_at_a70);
        if (ptr_at_a70 > 0x10000) {
            uint32_t env_val = *(uint32_t *)ptr_at_a70;
            log("[LevelLoad] *(param3+0xa70)=%p (CStaticDataEnv candidate)\n", (void *)env_val);

            // Log only — LoadData will be triggered later by on_spawn_monsters
            if (env_val > 0x10000) {
                log("[LevelLoad] env loaded_flag:%u\n", *(uint8_t *)env_val);
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
    log("[StartupInit] CALLED this:%p param2:%p param3:%p param4:%p\n",
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
    log("[LoadData] CALLED this(env):%p param2:%p param3:%p param4:%p\n",
        (void *)this_ptr, (void *)param_2, (void *)param_3, (void *)param_4);
    if (param_3 > 0x10000) {
        log("[LoadData] param3_str: %.128s\n", (char *)param_3);
    }
}

// --- stolen bytes ---

// crygame + 0x1223631: mov ebp,esp (8B EC) + mov eax,[ecx+0Ch] (8B 41 0C) = 5 bytes
static const uint8_t register_handler_stolen[] = {0x8B, 0xEC, 0x8B, 0x41, 0x0C};

// crygame + 0x1223148: push [ebp+0Ch] (FF 75 0C) + mov eax,[ecx] (8B 01) = 5 bytes
static const uint8_t call_handler_stolen[] = {0xFF, 0x75, 0x0C, 0x8B, 0x01};

// crygame + 0x12a2e10: push ebp (55) + mov ebp,esp (8B EC) + sub esp,1Ch (83 EC 1C) = 6 bytes
static const uint8_t spawn_monsters_stolen[] = {0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x1C};

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

static const JmpHookEntry crygame_jmp_hooks[] = {
    {0x1223631,  on_register_handler, register_handler_stolen, sizeof(register_handler_stolen)},
    {0x1223148,  on_call_handler,     call_handler_stolen,     sizeof(call_handler_stolen)},
    {0x12a2e10,  on_spawn_monsters,   spawn_monsters_stolen,   sizeof(spawn_monsters_stolen)},
    {0x50ab31,   on_log_notification, log_notification_stolen, sizeof(log_notification_stolen)},
    {0x1679c40,  on_staticdata_init,  staticdata_init_stolen,  sizeof(staticdata_init_stolen)},
    {0x1679cc0,  on_staticdata_set,   staticdata_set_stolen,   sizeof(staticdata_set_stolen)},
    {0x0c3f520,  on_level_load,       level_load_stolen,       sizeof(level_load_stolen)},
    {0x0b7ffd0,  on_startup_init,     startup_init_stolen,     sizeof(startup_init_stolen)},
    {0x169ed30,  on_load_data,        load_data_stolen,        sizeof(load_data_stolen)},
};

// --- install ---

void install_crygame_hooks() {
    DWORD base = wait_for_module("crygame");
    crygame_base = base;
    log("got crygame: %p \n", (void *)base);

    // Log initial state of CStaticDataEnv
    uint32_t env0 = *(uint32_t *)(base + OFF_DAT_12387c6c);
    uint32_t flag0 = *(uint32_t *)(base + OFF_DAT_12387c70);
    log("[init] CStaticDataEnv:%p flag:%u\n", (void *)env0, flag0);

    // Check if CStaticDataEnv was loaded (first byte = loaded flag)
    if (env0 > 0x10000) {
        uint8_t loaded_flag = *(uint8_t *)env0;
        log("[init] CStaticDataEnv->loaded_flag:%u\n", loaded_flag);
    }

    // Check if CGame::Init (FUN_10b7ffd0) prologue is still intact
    // Original bytes: 55 8B EC 81 EC E0 0B 00 00
    // If they're different, our hook overwrote them (shouldn't happen yet)
    // If they match, CGame::Init was NOT yet called-and-hooked-over
    uint8_t *cgame_init = (uint8_t *)(base + 0x0b7ffd0);
    log("[init] CGame::Init bytes: %02x %02x %02x %02x %02x\n",
        cgame_init[0], cgame_init[1], cgame_init[2], cgame_init[3], cgame_init[4]);

    // Check g_CMonsterInfo_SingletonPtr
    uint32_t moninfo0 = *(uint32_t *)(base + OFF_g_CMonsterInfo_Ptr);
    log("[init] g_CMonsterInfo_SingletonPtr:%p\n", (void *)moninfo0);

    // Dump CStaticDataEnv internal structure (first 32 bytes)
    if (env0 > 0x10000) {
        uint32_t *env_data = (uint32_t *)env0;
        log("[init] CStaticDataEnv dump: %08x %08x %08x %08x %08x %08x %08x %08x\n",
            env_data[0], env_data[1], env_data[2], env_data[3],
            env_data[4], env_data[5], env_data[6], env_data[7]);
    }

    org_fn_crygame_13EC290 = (fn_crygame_13EC290)(base + 0x13F3640);

    install_jmp_hooks(base, crygame_jmp_hooks, std::size(crygame_jmp_hooks));

}
