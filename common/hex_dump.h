#ifndef MHO_HEX_DUMP_H
#define MHO_HEX_DUMP_H

#include <cstdint>
#include <cstdio>
#include <string>

inline std::string to_hex(uint8_t *bytes, int size) {
    static const char hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    std::string str;
    for (int i = 0; i < size; ++i) {
        const char ch = bytes[i];
        str.append(&hex[(ch & 0xF0) >> 4], 1);
        str.append(&hex[ch & 0xF], 1);
        str.append(" ");
    }
    return str;
}

inline std::string to_ascii(uint8_t *bytes, int size) {
    std::string str;
    for (int i = 0; i < size; ++i) {
        const char ch = bytes[i];
        if (ch >= 32 && ch <= 127) {
            str.append(&ch, 1);
        } else {
            str.append(".");
        }
    }
    return str;
}

inline void show(uint8_t *bytes, int size) {
    fprintf(stdout, "\n");
    fprintf(stdout, "---------\n");
    fprintf(stdout, "Ptr:%p Size:%d\n", bytes, size);

    int chunk_size = 16;
    int chunks = size / chunk_size;
    int rem = size % chunk_size;

    int offset = 0;
    for (int i = 0; i < chunks; i++) {
        offset = i * chunk_size;
        fprintf(stdout, "0x%08X | %s| %s\n",
                offset,
                to_hex(&bytes[offset], chunk_size).c_str(),
                to_ascii(&bytes[offset], chunk_size).c_str()
        );
    }
    if (rem > 0) {
        offset = chunks * chunk_size;
        fprintf(stdout, "0x%08X | %s| %s\n",
                offset,
                to_hex(&bytes[offset], rem).c_str(),
                to_ascii(&bytes[offset], rem).c_str()
        );
    }

    fprintf(stdout, "---------\n");
    fprintf(stdout, "\n");
}

#endif
