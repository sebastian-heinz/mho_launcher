#include "mho_launcher_lib.h"
#include "memory.h"
#include "mho_types.h"
#include "cs_cmd.h"
#include "internal_notification_ids.h"
#include "util.h"
#include "tenproxy_tcls_sharedmememory.h"

#include "blockingconcurrentqueue.h"

#include <windows.h>
#include <fstream>
#include <thread>
#include <string>
#include <iostream>
#include <Tchar.h>

DWORD server_url_address = 0;

fn_crygame_13EC290 org_fn_crygame_13EC290 = nullptr;
fn_protocalhandler_log org_protocalhandler_log = nullptr;
fn_protocalhandler_log_format org_protocalhandler_format_log = nullptr;
fn_perform_tpdu_encryption org_perform_tpdu_encryption = nullptr;
fn_perform_tpdu_decryption org_perform_tpdu_decryption = nullptr;
fn_aes_key_expansion org_aes_key_expansion = nullptr;

bool log_crypto = false;

/// Event System - START
// Had some issues with printing console logs on game threads.
// Now only constructing the logs on game thread but not printing.
// Isolating the printing to my own thread via a queue.
struct Event {
    std::string msg;
};

std::atomic<bool> is_running = false;
moodycamel::BlockingConcurrentQueue<Event> *events = nullptr;

void run_events() {
    Event event{};
    while (is_running) {
        if (!events->wait_dequeue_timed(event, 500 * 1000)) {
            // check every 500ms if we are still running
            continue;
        }
        std::cout << event.msg;
    }
}

void log(const char *fmt, ...) {
    va_list ap;
            va_start (ap, fmt);
    std::string buf = vformat(fmt, ap);
            va_end (ap);
    events->enqueue({buf});
}

/// Event System - END

void __cdecl protocalhandler_log(
        int p_unk,
        size_t p_buffer_size,
        wchar_t *p_str,
        void *p_str_fmt_args
) {
    // call original, just in case we find a switch that prints to file or want to observe behaviour related.
    org_protocalhandler_log(p_unk, p_buffer_size, p_str, p_str_fmt_args);

    size_t w_str_size = std::wcslen(p_str);
    if (w_str_size <= 0) {
        // TODO I am not interested if there is no string content in the buffer,
        // TODO however it gets called some times without any content.
        //fprintf("protocalhandler::w_str_size:%d (p_unk:%d, p_buffer_size:%d p_str:%p, p_str_fmt_args:%p)\n",
        //        w_str_size, p_unk, p_buffer_size, p_str, p_str_fmt_args
        //);
        return;
    }
    // `p_buffer_size` should be appropriate sized to hold formatted string
    size_t out_buffer_size = p_buffer_size + w_str_size + 1024; // just to be sure
    wchar_t *w_str_fmt = new wchar_t[out_buffer_size];

    // this is a function the game uses to apply formatting, it als adds process and thread id to the string
    org_protocalhandler_format_log(w_str_fmt, out_buffer_size, p_str, p_str_fmt_args);
    size_t w_str_fmt_size = std::wcslen(w_str_fmt);
    std::wstring w_log_text(w_str_fmt, w_str_fmt_size);

    // converting wstring to string, to be able to print it to console
    std::string log_text = ws_2_s(w_log_text);
    delete[] w_str_fmt;
    log("protocalhandler_log:%s\n", log_text.c_str());
}

const int IGNORE_LOGS_SIZE = 9;
std::string IGNORE_LOGS[IGNORE_LOGS_SIZE] = {
        "$4[Error] I3DEngine::LoadStatObj: filename is not specified",
        "$6[Warning] Texture does not exist",
        "$6[Warning] Warning: CTexMan::ImagePreprocessing",
        "$6[Warning] Error: CTexMan::ImagePreprocessing:",
        "$6[Warning] CAF-File Not Found:",
        "$6[Warning] Normal map should have '_ddn' suffix in filename",
        "$6[Warning] CryAnimation: process Aimpose",
        "$6[Warning] Reference if not existing Joint-Name. Aim-IK disabled",
        "$6[Warning] Failed to create animation alias"
};

void client_log(int do_log, char *near_log_ptr) {
    if (do_log == 0) {
        return;
    }
    char *log_ptr = near_log_ptr + 0x20;
    int log_len = 0;
    while (true) {
        if (log_ptr[log_len] == 0) {
            break;
        }
        log_len++;
    }
    if (log_len <= 0) {
        return;
    }
    std::string log_text = std::string(log_ptr, log_len);

    for (int i = 0; i < IGNORE_LOGS_SIZE; i++) {
        if (log_text.find(IGNORE_LOGS[i]) != std::string::npos) {
            return;
        }
    }

    log("client_log: %s \n", log_text.c_str());
}

void tenproxy_log(wchar_t *log_ptr) {
    std::wstring w_log_txt = std::wstring(log_ptr);
    std::string log_text = ws_2_s(w_log_txt);
    log("tenproxy_log: %s \n", log_text.c_str());
}

/**
 * This is the first function that was easy to hook around the server connection routine.
 * its only purpose is to alter memory at the time, right before it is used.
 */
void __cdecl crygame_13F3640() {

    log("crygame_13F3640\n");
    const char *url = "127.0.0.1:8142";
    //const char *url = "127.0.0.1:8142,127.0.0.1:8142,127.0.0.1:8142,vport:8142";
    WriteMemory((LPVOID) server_url_address, url, strlen(url));
    org_fn_crygame_13EC290();
}

int __cdecl aes_key_expansion(
        void *key,
        unsigned int key_len_bits,
        void *expanded_key
) {
    log("aes_key_expansion (bits:%d)\n", key_len_bits);

    unsigned int key_len_bytes = key_len_bits / 8;
    show((uint8_t *) key, key_len_bytes);

    int ret = org_aes_key_expansion(key, key_len_bits, expanded_key);
    if (key_len_bits == 128) {
        unsigned int expanded_key_len_bytes = 176;
        show((uint8_t *) expanded_key, expanded_key_len_bytes);
    }
    return ret;
}


int __cdecl perform_tpdu_decryption(
        TQQApiHandle *apiHandle,
        char *inputBuffer,
        unsigned int inputBufferLength,
        void **outputBuffer,
        unsigned int *outputBufferLength,
        int is_TPDU_CMD_PLAIN,
        int allow_unencrypted_packets) {

    uint8_t *encryption_mode_addr = (uint8_t *) apiHandle + 0x84;
    // *encryption_mode_addr = 0;
    // allow_unencrypted_packets = 1;
    if (log_crypto) {
        log("DECRYPT - START\n");
        log("DECRYPT - encryption_mode_addr: %d\n", *encryption_mode_addr);
        log("DECRYPT - Input Buffer\n");
        show((uint8_t *) inputBuffer, inputBufferLength);
    }

    int ret = org_perform_tpdu_decryption(apiHandle,
                                          inputBuffer,
                                          inputBufferLength,
                                          outputBuffer,
                                          outputBufferLength,
                                          is_TPDU_CMD_PLAIN,
                                          allow_unencrypted_packets
    );

    void *out = *outputBuffer;
    signed int outlen = *outputBufferLength;

    if (log_crypto) {
        log("DECRYPT - Return: Dec:%d Hex:0x%08X\n", ret, ret);
        log("DECRYPT - Output Buffer\n");
        show((uint8_t *) out, outlen);
        log("DECRYPT - END\n");
    }

    return ret;
}

void __cdecl register_handler(
        uint32_t packet_id,
        HandlerCallbackDefintion *handler_definition
) {
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
            packet_id,
            cmd.name.c_str(),
            handler_definition->handler_callback_function_ptr,
            handler_definition->unknown_field
        );
    } else {
        log("register_handler !!! NOT FOUND !!! PacketId: %u \n", packet_id);
    }
}

void __cdecl call_handler(
        void *call_fn,
        HandlerCallbackDefintion *handler_definition,
        uint32_t packet_id,
        uint8_t *packet_data
) {
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
            packet_id,
            cmd.name.c_str(),
            handler_definition->handler_callback_function_ptr,
            handler_definition->unknown_field,
            call_fn
        );
    } else {
        log("call_handler: !!! NOT FOUND !!!  CMD:%u %s handler_fn:%p, unk:%u, call_fn:%p\n",
            packet_id,
            cmd.name.c_str(),
            handler_definition->handler_callback_function_ptr,
            handler_definition->unknown_field,
            call_fn
        );
    }
}

void __cdecl log_handle_logic_or_game_event_notification(
    char* event_cstr,
    InternalEventNotification* notif,
    uint32_t unk_group_index
) {

    std::string notification_type = "unknown";
    std::string event = event_cstr;
    if (event.find("handleLogicNotification") != std::string::npos) {
        notification_type = GetMHLogicEventIDName(static_cast<MHLogicEventID>(notif->notification_id));
    }
    else if (event.find("handleGameNotification") != std::string::npos) {
        notification_type = GetMHGameEventIDName(static_cast<MHGameEventID>(notif->notification_id));
    }

    log("log_handle_logic_or_game_event_notification(event:%s, notification_id:%u (type: %s), unk_group_idx: %u)\n",
        event_cstr,
        notif->notification_id,
        notification_type.c_str(),
        unk_group_index
    );
}

int __cdecl perform_tpdu_encryption(
        TQQApiHandle *apiHandle,
        void *inputBuffer,
        signed int inputBufferLength,
        void **outputBuffer,
        signed int *outputBufferLength,
        int allow_unencrypted
) {

    uint8_t *encryption_mode_addr = (uint8_t *) apiHandle + 0x84;
    //*encryption_mode_addr = 0;
    //allow_unencrypted = 1;

    if (log_crypto) {
        log("ENCRYPT - START\n");
        log("ENCRYPT - Input Buffer\n");
        show((uint8_t *) inputBuffer, inputBufferLength);
        log("ENCRYPT - encryption_mode_addr: %d\n", *encryption_mode_addr);
    }

    int ret = org_perform_tpdu_encryption(apiHandle,
                                          inputBuffer,
                                          inputBufferLength,
                                          outputBuffer,
                                          outputBufferLength,
                                          allow_unencrypted
    );

    void *out = *outputBuffer;
    signed int outlen = *outputBufferLength;

    if (log_crypto) {
        log("ENCRYPT - Return: Dec:%d Hex:0x%08X\n", ret, ret);
        log("ENCRYPT - Output Buffer\n");
        show((uint8_t *) out, outlen);
        log("ENCRYPT - END\n");
    }

    return ret;
}


// @formatter:off
_declspec(naked)
void asm_client_log() {
    _asm
    {
        pushad
        mov eax, esp
        push eax
        push ecx
        call client_log
        add esp, 0x8
        popad
        // recover stolen bytes
        mov esp, ebp
        pop ebp
        ret 0xC
    }
}

_declspec(naked)
void asm_tenproxy_log() {
    _asm
    {
        pushad
        push eax
        call tenproxy_log
        add esp, 0x4
        popad
        // recover stolen bytes
        pop edi
        pop esi
        pop ebx
        mov esp,ebp
        pop ebp
        ret 0x14
    }
}

DWORD register_handler_ret_jmp;
_declspec(naked)
void asm_register_handler() {
    _asm
    {
        pushad
        mov ebx, [esp + 0x28]
        mov eax, [esp + 0x2C]
        push eax
        push ebx
        call register_handler
        add esp, 0x8
        popad
        // recover stolen bytes
        mov ebp, esp
        mov eax, dword ptr ds:[ecx+0xC]
        jmp register_handler_ret_jmp
    }
}

DWORD call_handler_ret_jmp;
//DWORD call_handler_hook_ptr;
//DWORD call_handler_hook;
_declspec(naked)
void asm_call_handler() {
    _asm
    {
        pushad
        push dword ptr ss:[ebp+0xC]
        push esi
        push dword ptr ss:[ebp+0x8]
        mov eax, dword ptr ds:[ecx]
        push dword ptr ds:[eax]
        call call_handler
        add esp, 0x10
        popad
        push dword ptr ss:[ebp+0xC]
        mov eax, dword ptr ds:[ecx]
        jmp call_handler_ret_jmp
    }
}

DWORD log_handle_logic_or_game_event_notification_ret_jmp;
_declspec(naked)
void asm_log_handle_logic_or_game_event_notification() {
    _asm
    {
        pushad
        mov ecx, [esp + 0x28]
        mov ebx, [esp + 0x2C]
        mov eax, [esp + 0x30]
        push eax
        push ebx
        push ecx
        call log_handle_logic_or_game_event_notification
        add esp, 0xc
        popad
        // recover stolen bytes
        mov ebp, esp
        sub esp, 18h
        jmp log_handle_logic_or_game_event_notification_ret_jmp
    }
}

// @formatter:on


/**
 * waits until crygame.dll is loaded and performs and applies patches to its memory
 */
void run_crygame() {
    HMODULE crygame_handle = nullptr;
    DWORD crygame_addr = 0;
    log("wait for crygame... \n");
    while (!crygame_handle) {
        crygame_handle = GetModuleHandleA("crygame");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    crygame_addr = (DWORD) crygame_handle;
    log("got crygame_handle: %p \n", crygame_handle);

    // assign original function calls
    org_fn_crygame_13EC290 = (fn_crygame_13EC290) (crygame_addr + 0x13F3640);

    // hook existing ones
    //hook_call(crygame_addr, 0x11AED64, &crygame_13F3640);


    // listen to register packet handler
    patch_jmp(crygame_addr, 0x1223631, &asm_register_handler);
    register_handler_ret_jmp = crygame_addr + 0x1223636;


    patch_jmp(crygame_addr, 0x1223148, &asm_call_handler);
    call_handler_ret_jmp = crygame_addr + 0x122314D;
    //call_handler_hook = (DWORD) call_handler;
    //call_handler_hook_ptr = (DWORD) &call_handler_hook;

    patch_jmp(crygame_addr, 0x50ab31, &asm_log_handle_logic_or_game_event_notification);
    log_handle_logic_or_game_event_notification_ret_jmp = crygame_addr + 0x50ab36;

}


/**
 * waits until tenproxy.dll is loaded and performs and applies patches to its memory
 */
void run_tenproxy() {
    HMODULE tenproxy_handle = nullptr;
    DWORD tenproxy_addr = 0;
    log("wait for tenproxy... \n");
    while (!tenproxy_handle) {
        tenproxy_handle = GetModuleHandleA("tenproxy.dll");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    tenproxy_addr = (DWORD) tenproxy_handle;
    log("got tenproxy_handle: %p \n", tenproxy_handle);

    // capture tenproxy logs
    patch_jmp(tenproxy_addr, 0xA143, &asm_tenproxy_log);

    // disable conditional logging
    patch_nop(tenproxy_addr, 0xA0D3, 8);
}

/**
 * waits until protocalhandler.dll is loaded and performs and applies patches to its memory
 */
void run_protocal_handler() {
    HMODULE protocal_handler_handle = nullptr;

    log("wait for protocalhandler... \n");
    while (!protocal_handler_handle) {
        protocal_handler_handle = GetModuleHandleA("protocalhandler");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    DWORD protocal_handler_addr = (DWORD) protocal_handler_handle;
    log("got protocal_handler_handle: %p \n", protocal_handler_handle);

    // assign original function calls
    org_protocalhandler_log = (fn_protocalhandler_log) (protocal_handler_addr + 0x1703);
    org_protocalhandler_format_log = (fn_protocalhandler_log_format) (protocal_handler_addr + 0x1A96);

    org_perform_tpdu_decryption = (fn_perform_tpdu_decryption) (protocal_handler_addr + 0x73DC0);
    org_perform_tpdu_encryption = (fn_perform_tpdu_encryption) (protocal_handler_addr + 0x73bb0);
    org_aes_key_expansion = (fn_aes_key_expansion) (protocal_handler_addr + 0x888E0);

    hook_call(protocal_handler_addr, 0x36002, &perform_tpdu_decryption);
    hook_call(protocal_handler_addr, 0x360FE, &perform_tpdu_decryption);
    hook_call(protocal_handler_addr, 0x74AD3, &perform_tpdu_decryption);
    hook_call(protocal_handler_addr, 0x74F7F, &perform_tpdu_decryption);
    hook_call(protocal_handler_addr, 0x75336, &perform_tpdu_decryption);
    hook_call(protocal_handler_addr, 0x75508, &perform_tpdu_decryption);
    hook_call(protocal_handler_addr, 0x75651, &perform_tpdu_decryption);

    hook_call(protocal_handler_addr, 0x36FAB, &perform_tpdu_encryption);
    hook_call(protocal_handler_addr, 0x742A2, &perform_tpdu_encryption);
    hook_call(protocal_handler_addr, 0x74661, &perform_tpdu_encryption);
    hook_call(protocal_handler_addr, 0x75B70, &perform_tpdu_encryption);
    hook_call(protocal_handler_addr, 0x76069, &perform_tpdu_encryption);

    hook_call(protocal_handler_addr, 0x88CB0, &aes_key_expansion);
    hook_call(protocal_handler_addr, 0x8B1E1, &aes_key_expansion);
    hook_call(protocal_handler_addr, 0x8B50B, &aes_key_expansion);

    hook_call(protocal_handler_addr, 0x39171, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x39141, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x390B1, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x390E1, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x3910E, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x391A1, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x38F2E, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x38F61, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x38F04, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x123F1, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x12391, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x123C1, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x1CC61, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x38FBE, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x39021, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x38F91, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x3907E, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x208D1, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x39051, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x38FF1, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x1CC91, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0xC921, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0xC551, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x83F1, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0xA831, &protocalhandler_log);
    hook_call(protocal_handler_addr, 0x83C1, &protocalhandler_log);
}

void CreateConsole() {
    if (!AllocConsole()) {
        return;
    }

    // std::cout, std::clog, std::cerr, std::cin
    FILE *fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();

    // std::wcout, std::wclog, std::wcerr, std::wcin
    HANDLE hConOut = CreateFile(_T("CONOUT$"),
                                GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL
    );
    HANDLE hConIn = CreateFile(_T("CONIN$"),
                               GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL
    );
    SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
    SetStdHandle(STD_ERROR_HANDLE, hConOut);
    SetStdHandle(STD_INPUT_HANDLE, hConIn);
    std::wcout.clear();
    std::wclog.clear();
    std::wcerr.clear();
    std::wcin.clear();

    std::ios::sync_with_stdio();
}


void run() {
    events = new moodycamel::BlockingConcurrentQueue<Event>(100);
    is_running = true;
    CreateConsole();

    new std::thread(run_events);
    log("run\n");

    TenProxyTclsSharedMeMemory *tptsmm = new TenProxyTclsSharedMeMemory();
    tptsmm->map(0);


    std::wstring exe_name_w = get_exe_name();
    std::string exe_name = ws_2_s(exe_name_w);
    log("exe_name: %s \n", exe_name.c_str());

    // get base addr
    HMODULE mho_client_handle = GetModuleHandleW(exe_name_w.c_str());
    DWORD mho_client_addr = (DWORD) mho_client_handle;
    log("mho_client_handle: %p \n", mho_client_handle);

    // assign variables depending on mhoclient base
    server_url_address = mho_client_addr + 0x157AAA0; // RVA
    log("server_url_address: 0x%08X \n", server_url_address);

    // capture mho client logs
    patch_jmp(mho_client_addr, 0x3E0F06, &asm_client_log);
    // disable conditional logging
    patch_nop(mho_client_addr, 0x3E0C82, 6);

    new std::thread(run_crygame);
    new std::thread(run_protocal_handler);
    new std::thread(run_tenproxy);
}

BOOL WINAPI DllMain(HINSTANCE h_instance, DWORD fdw_reason, LPVOID lpv_reserved) {
    switch (fdw_reason) {
        case DLL_PROCESS_ATTACH:
            new std::thread(run);
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            is_running = false;
            break;
    }
    return TRUE;
}