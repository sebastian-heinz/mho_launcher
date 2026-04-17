#include "hooks_protocalhandler.h"
#include "log.h"
#include "hook_table.h"
#include "hex_dump.h"
#include "str_util.h"

#include "mho_types.h"

#include <cstdint>
#include <string>
#include <cwchar>

// --- original function pointers ---

static fn_protocalhandler_log org_protocalhandler_log = nullptr;
static fn_protocalhandler_log_format org_protocalhandler_format_log = nullptr;
static fn_perform_tpdu_encryption org_perform_tpdu_encryption = nullptr;
static fn_perform_tpdu_decryption org_perform_tpdu_decryption = nullptr;
static fn_aes_key_expansion org_aes_key_expansion = nullptr;

static bool log_crypto = false;

// --- callbacks ---

static void __cdecl protocalhandler_log(
        int p_unk,
        size_t p_buffer_size,
        wchar_t *p_str,
        void *p_str_fmt_args) {
    org_protocalhandler_log(p_unk, p_buffer_size, p_str, p_str_fmt_args);

    size_t w_str_size = std::wcslen(p_str);
    if (w_str_size <= 0) {
        return;
    }
    size_t out_buffer_size = p_buffer_size + w_str_size + 1024;
    wchar_t *w_str_fmt = new wchar_t[out_buffer_size];

    org_protocalhandler_format_log(w_str_fmt, out_buffer_size, p_str, p_str_fmt_args);
    size_t w_str_fmt_size = std::wcslen(w_str_fmt);
    std::wstring w_log_text(w_str_fmt, w_str_fmt_size);

    std::string log_text = ws_2_s(w_log_text);
    delete[] w_str_fmt;
    log("protocalhandler_log:%s\n", log_text.c_str());
}

static int __cdecl aes_key_expansion(
        void *key,
        unsigned int key_len_bits,
        void *expanded_key) {
    log("aes_key_expansion (bits:%d)\n", key_len_bits);

    unsigned int key_len_bytes = key_len_bits / 8;
    show((uint8_t *) key, key_len_bytes);

    int ret = org_aes_key_expansion(key, key_len_bits, expanded_key);
    if (key_len_bits == 128) {
        show((uint8_t *) expanded_key, 176);
    }
    return ret;
}

static int __cdecl perform_tpdu_decryption(
        TQQApiHandle *apiHandle,
        char *inputBuffer,
        unsigned int inputBufferLength,
        void **outputBuffer,
        unsigned int *outputBufferLength,
        int is_TPDU_CMD_PLAIN,
        int allow_unencrypted_packets) {

    uint8_t *encryption_mode_addr = (uint8_t *) apiHandle + 0x84;
    if (log_crypto) {
        log("DECRYPT - START\n");
        log("DECRYPT - encryption_mode_addr: %d\n", *encryption_mode_addr);
        log("DECRYPT - Input Buffer\n");
        show((uint8_t *) inputBuffer, inputBufferLength);
    }

    int ret = org_perform_tpdu_decryption(apiHandle, inputBuffer, inputBufferLength,
                                          outputBuffer, outputBufferLength,
                                          is_TPDU_CMD_PLAIN, allow_unencrypted_packets);

    if (log_crypto) {
        void *out = *outputBuffer;
        signed int outlen = *outputBufferLength;
        log("DECRYPT - Return: Dec:%d Hex:0x%08X\n", ret, ret);
        log("DECRYPT - Output Buffer\n");
        show((uint8_t *) out, outlen);
        log("DECRYPT - END\n");
    }

    return ret;
}

static int __cdecl perform_tpdu_encryption(
        TQQApiHandle *apiHandle,
        void *inputBuffer,
        signed int inputBufferLength,
        void **outputBuffer,
        signed int *outputBufferLength,
        int allow_unencrypted) {

    uint8_t *encryption_mode_addr = (uint8_t *) apiHandle + 0x84;
    if (log_crypto) {
        log("ENCRYPT - START\n");
        log("ENCRYPT - Input Buffer\n");
        show((uint8_t *) inputBuffer, inputBufferLength);
        log("ENCRYPT - encryption_mode_addr: %d\n", *encryption_mode_addr);
    }

    int ret = org_perform_tpdu_encryption(apiHandle, inputBuffer, inputBufferLength,
                                          outputBuffer, outputBufferLength,
                                          allow_unencrypted);

    if (log_crypto) {
        void *out = *outputBuffer;
        signed int outlen = *outputBufferLength;
        log("ENCRYPT - Return: Dec:%d Hex:0x%08X\n", ret, ret);
        log("ENCRYPT - Output Buffer\n");
        show((uint8_t *) out, outlen);
        log("ENCRYPT - END\n");
    }

    return ret;
}

// --- hook tables ---

static const CallHookEntry decryption_hooks[] = {
    {0x36002, (void*)&perform_tpdu_decryption},
    {0x360FE, (void*)&perform_tpdu_decryption},
    {0x74AD3, (void*)&perform_tpdu_decryption},
    {0x74F7F, (void*)&perform_tpdu_decryption},
    {0x75336, (void*)&perform_tpdu_decryption},
    {0x75508, (void*)&perform_tpdu_decryption},
    {0x75651, (void*)&perform_tpdu_decryption},
};

static const CallHookEntry encryption_hooks[] = {
    {0x36FAB, (void*)&perform_tpdu_encryption},
    {0x742A2, (void*)&perform_tpdu_encryption},
    {0x74661, (void*)&perform_tpdu_encryption},
    {0x75B70, (void*)&perform_tpdu_encryption},
    {0x76069, (void*)&perform_tpdu_encryption},
};

static const CallHookEntry aes_hooks[] = {
    {0x88CB0, (void*)&aes_key_expansion},
    {0x8B1E1, (void*)&aes_key_expansion},
    {0x8B50B, (void*)&aes_key_expansion},
};

static const CallHookEntry log_hooks[] = {
    {0x39171, (void*)&protocalhandler_log},
    {0x39141, (void*)&protocalhandler_log},
    {0x390B1, (void*)&protocalhandler_log},
    {0x390E1, (void*)&protocalhandler_log},
    {0x3910E, (void*)&protocalhandler_log},
    {0x391A1, (void*)&protocalhandler_log},
    {0x38F2E, (void*)&protocalhandler_log},
    {0x38F61, (void*)&protocalhandler_log},
    {0x38F04, (void*)&protocalhandler_log},
    {0x123F1, (void*)&protocalhandler_log},
    {0x12391, (void*)&protocalhandler_log},
    {0x123C1, (void*)&protocalhandler_log},
    {0x1CC61, (void*)&protocalhandler_log},
    {0x38FBE, (void*)&protocalhandler_log},
    {0x39021, (void*)&protocalhandler_log},
    {0x38F91, (void*)&protocalhandler_log},
    {0x3907E, (void*)&protocalhandler_log},
    {0x208D1, (void*)&protocalhandler_log},
    {0x39051, (void*)&protocalhandler_log},
    {0x38FF1, (void*)&protocalhandler_log},
    {0x1CC91, (void*)&protocalhandler_log},
    {0xC921,  (void*)&protocalhandler_log},
    {0xC551,  (void*)&protocalhandler_log},
    {0x83F1,  (void*)&protocalhandler_log},
    {0xA831,  (void*)&protocalhandler_log},
    {0x83C1,  (void*)&protocalhandler_log},
};

// --- install ---

void install_protocalhandler_hooks() {
    DWORD base = wait_for_module("protocalhandler");
    log("got protocalhandler: %p \n", (void*)base);

    org_protocalhandler_log = (fn_protocalhandler_log)(base + 0x1703);
    org_protocalhandler_format_log = (fn_protocalhandler_log_format)(base + 0x1A96);
    org_perform_tpdu_decryption = (fn_perform_tpdu_decryption)(base + 0x73DC0);
    org_perform_tpdu_encryption = (fn_perform_tpdu_encryption)(base + 0x73bb0);
    org_aes_key_expansion = (fn_aes_key_expansion)(base + 0x888E0);

    install_call_hooks(base, decryption_hooks, std::size(decryption_hooks));
    install_call_hooks(base, encryption_hooks, std::size(encryption_hooks));
    install_call_hooks(base, aes_hooks, std::size(aes_hooks));
    install_call_hooks(base, log_hooks, std::size(log_hooks));
}
