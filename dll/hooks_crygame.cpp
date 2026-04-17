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
        log("call_handler: CMD:%u %s handler_fn:%p, unk:%u, call_fn:%p\n",
            packet_id, cmd.name.c_str(),
            handler_definition->handler_callback_function_ptr,
            handler_definition->unknown_field, call_fn);
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
    // Original asm read: [ebp+0x8], esi, [ebp+0xC], [ecx]→[eax]
    uint32_t packet_id = *(uint32_t *)(ctx->ebp + 0x8);
    HandlerCallbackDefintion *handler_def = (HandlerCallbackDefintion *)ctx->esi;
    uint8_t *packet_data = *(uint8_t **)(ctx->ebp + 0xC);
    void *vtable = *(void **)(ctx->ecx);
    void *call_fn = *(void **)vtable;
    call_handler(call_fn, handler_def, packet_id, packet_data);
}

static void on_log_notification(HookContext *ctx) {
    // Original asm read from [esp+0x28], [esp+0x2C], [esp+0x30]
    DWORD *stack = hook_stack(ctx);
    char *event_cstr = (char *)stack[2];                                // [original_esp + 0x8]
    InternalEventNotification *notif = (InternalEventNotification *)stack[3];  // [original_esp + 0xC]
    uint32_t unk_group_index = stack[4];                                // [original_esp + 0x10]
    log_handle_notification(event_cstr, notif, unk_group_index);
}

// --- stolen bytes ---

// crygame + 0x1223631: mov ebp,esp (8B EC) + mov eax,[ecx+0Ch] (8B 41 0C) = 5 bytes
static const uint8_t register_handler_stolen[] = {0x8B, 0xEC, 0x8B, 0x41, 0x0C};

// crygame + 0x1223148: push [ebp+0Ch] (FF 75 0C) + mov eax,[ecx] (8B 01) = 5 bytes
static const uint8_t call_handler_stolen[] = {0xFF, 0x75, 0x0C, 0x8B, 0x01};

// crygame + 0x50ab31: mov ebp,esp (8B EC) + sub esp,18h (83 EC 18) = 5 bytes
static const uint8_t log_notification_stolen[] = {0x8B, 0xEC, 0x83, 0xEC, 0x18};

static const JmpHookEntry crygame_jmp_hooks[] = {
    {0x1223631, on_register_handler, register_handler_stolen, sizeof(register_handler_stolen)},
    {0x1223148, on_call_handler,     call_handler_stolen,     sizeof(call_handler_stolen)},
    {0x50ab31,  on_log_notification, log_notification_stolen, sizeof(log_notification_stolen)},
};

// --- install ---

void install_crygame_hooks() {
    DWORD base = wait_for_module("crygame");
    log("got crygame: %p \n", (void *)base);

    org_fn_crygame_13EC290 = (fn_crygame_13EC290)(base + 0x13F3640);

    install_jmp_hooks(base, crygame_jmp_hooks, std::size(crygame_jmp_hooks));
}
