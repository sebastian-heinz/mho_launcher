#ifndef MHO_LAUNCHER_MHO_TYPES_H
#define MHO_LAUNCHER_MHO_TYPES_H

#include <cstdint>

typedef void *TQQApiHandle;

typedef int (__cdecl *fn_perform_tpdu_encryption)(
        TQQApiHandle *apiHandle,
        void *inputBuffer,
        signed int inputBufferLength,
        void **outputBuffer,
        signed int *outputBufferLength,
        int allow_unencrypted
);

typedef int (__cdecl *fn_perform_tpdu_decryption)(
        TQQApiHandle *apiHandle,
        char *inputBuffer,
        unsigned int inputBufferLength,
        void **outputBuffer,
        unsigned int *outputBufferLength,
        int is_TPDU_CMD_PLAIN,
        int allow_unencrypted_packets
);

typedef int (__cdecl *fn_aes_key_expansion)(
        void *key,
        unsigned int key_len_bits,
        void *expanded_key
);

typedef void (__cdecl *fn_protocalhandler_log)(
        int p_unk,
        size_t p_buffer_size,
        wchar_t *p_str,
        void *p_str_fmt_args
);

struct HandlerCallbackDefintion {
    uint32_t vftable_ptr;                 /* 0x00 */
    uint32_t mgr_instance_ptr;            /* 0x04 */
    void* handler_callback_function_ptr;  /* 0x08 */
    uint32_t unknown_field;               /* 0x0C */
};


/***
 * p_out_str_fmt - out buffer to hold formatted string
 * p_out_str_fmt_size - size of out buffer
 * p_str - string to be formatted
 * p_str_fmt_args - format arguents
 */
typedef void (__cdecl *fn_protocalhandler_log_format)(
        wchar_t *p_out_str_fmt,
        size_t p_out_str_fmt_size,
        wchar_t *p_str,
        void *p_str_fmt_args
);

typedef void(__cdecl *fn_crygame_13EC290)();

#endif //MHO_LAUNCHER_MHO_TYPES_H