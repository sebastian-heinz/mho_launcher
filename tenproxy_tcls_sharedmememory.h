#ifndef MHO_LAUNCHER_TENPROXY_TCLS_SHAREDMEMEMORY_H
#define MHO_LAUNCHER_TENPROXY_TCLS_SHAREDMEMEMORY_H

#include "ag_buffer.h"
#include "util.h"

#include <thread>
#include <string>

class TenProxyTclsSharedMeMemory {

private:
    const int OFFSET_TABLE_SIZE = 64;
    const int VAR_20_OFFSET_TABLE_SIZE = 17;

    std::string connection;
    std::string ip;
    std::string tcls_client_exe;
    std::string var_33;
    std::string var_19;
    std::string var_20_v5;
    std::string var_20_v9;
    std::string var_15;
    uint8_t *aes_key;
    size_t aes_key_size;
    uint8_t *var_60;
    size_t var_60_size;
    uint8_t *var_4;
    size_t var_4_size;
    uint8_t *var_27;
    size_t var_27_size;
    uint8_t *var_25;
    size_t var_25_size;

public:
    TenProxyTclsSharedMeMemory() {
        //connection = "127.0.0.1:8140;127.0.0.1:8141;127.0.0.1:8142;vport:8143";
        connection = "127.0.0.1:8142";
        ip = "127.0.0.1";
        // tcls_client_exe = "C:\\Games\\Monster Hunter Online\\TCLS\\client.exe";
        tcls_client_exe = "";
        aes_key_size = 16;
        aes_key = new uint8_t[aes_key_size]{
                0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
        };
        var_33 = "1:双线一区;2:双线二区;3:双线三区;";
        var_19 = "dir_root;双线三区;283缘聚仙侠;";
        var_20_v5 = "283缘聚仙侠";
        var_20_v9 = "-s:3";
        var_15 = "QQ仙侠传登录程序6";
        var_60_size = 16;
        var_60 = new uint8_t[var_60_size]{
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        var_4_size = 96;
        var_4 = new uint8_t[var_4_size];
        memset(var_4, 0, var_4_size);

        var_27_size = 72;
        var_27 = new uint8_t[var_27_size];
        memset(var_27, 0, var_27_size);

        var_25_size = 16;
        var_25 = new uint8_t[var_25_size]; // was 16 0x00 bytes
        memset(var_25, 0, var_25_size);
    }

    uint32_t var_offset_table_pos;
    uint32_t var_1_offset;
    uint32_t var_2_offset;
    uint32_t var_3_offset;
    uint32_t var_4_offset;
    uint32_t var_5_aes_key_offset;
    uint32_t var_6_offset;
    uint32_t var_7_offset;
    uint32_t var_14_tcls_client_exe_offset;
    uint32_t var_15_offset;
    uint32_t var_16_offset;
    uint32_t var_17_offset;
    uint32_t var_18_offset;
    uint32_t var_19_offset;
    uint32_t var_20_offset;
    uint32_t var_21_offset;
    uint32_t var_22_offset;
    uint32_t var_23_offset;
    uint32_t var_24_offset;
    uint32_t var_25_offset;
    uint32_t var_26_offset;
    uint32_t var_27_offset;
    uint32_t var_32_offset;
    uint32_t var_33_offset;
    uint32_t var_36_offset;
    uint32_t var_37_offset;
    uint32_t var_40_offset;
    uint32_t var_41_offset;
    uint32_t var_42_offset;
    uint32_t var_60_offset;

    uint32_t var_20_offset_table_pos;
    uint32_t var_20_v2_connection_offset;
    uint32_t var_20_v3_ip_offset;
    uint32_t var_20_v4_offset;
    uint32_t var_20_v5_offset;
    uint32_t var_20_v6_offset;
    uint32_t var_20_v7_offset;
    uint32_t var_20_v8_offset;
    uint32_t var_20_v9_offset;
    uint32_t var_20_v10_offset;

    size_t write(uint8_t *p_dst, size_t p_dst_len) {
        AgBuffer *buf = new AgBuffer();
        var_offset_table_pos = buf->get_position();
        buf->write_u32(0); // reserve offset table size
        buf->write_repeat(0, OFFSET_TABLE_SIZE * 4); // reserve offset table

        // var_1
        var_1_offset = buf->get_position();
        buf->write_u32(4);
        buf->write_u32(0x300A01);

        // var_2
        var_2_offset = buf->get_position();
        buf->write_u32(4);
        buf->write_u32(0x14924512);

        // var_6
        var_6_offset = buf->get_position();
        buf->write_u32(4);
        buf->write_u32(0x14924512);

        // var_3
        var_3_offset = buf->get_position();
        size_t unk_2_length = 0x60;
        buf->write_u32(0x1);
        buf->write_u8(unk_2_length);

        // var_4
        var_4_offset = buf->get_position();
        buf->write_u32(var_4_size);
        buf->write_data(var_4, var_4_size);

        // var_5_aes_key_offset
        var_5_aes_key_offset = buf->get_position();
        buf->write_u32(aes_key_size);
        buf->write_data(aes_key, aes_key_size);

        // var_21
        var_21_offset = buf->get_position();
        buf->write_u32(0x1);
        buf->write_u8(0x0);

        // var_22
        var_22_offset = buf->get_position();
        buf->write_u32(0x0);

        // var_23
        var_23_offset = buf->get_position();
        buf->write_u32(0x1);
        buf->write_u8(0x0);

        // var_24
        var_24_offset = buf->get_position();
        buf->write_u32(0x0);

        // var_25
        var_25_offset = buf->get_position();
        buf->write_u32(var_25_size);
        buf->write_data(var_25, var_25_size);

        // var_26
        var_26_offset = buf->get_position();
        size_t unk_11_length = 0x48;
        buf->write_u32(0x1);
        buf->write_u8(unk_11_length);

        // var_27
        var_27_offset = buf->get_position();
        buf->write_u32(var_27_size);
        buf->write_data(var_27, var_27_size);

        // var_32
        var_32_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x3);

        // var_33
        var_33_offset = buf->get_position();

        std::wstring var_33_utf16 = get_utf16(var_33);
        int var_33_len = var_33_utf16.size() * sizeof(wchar_t);
        buf->write_u32(var_33_len);
        buf->write_wstring(var_33_utf16);

        // var_17
        var_17_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x1);

        // var_18
        var_18_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x1);

        // var_19
        var_19_offset = buf->get_position();
        std::wstring var_19_utf16 = get_utf16(var_19);
        int var_19_len = var_19_utf16.size() * sizeof(wchar_t);
        buf->write_u32(var_19_len);
        buf->write_wstring(var_19_utf16);

        // var_36
        var_36_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x1);

        // var_37
        var_37_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x0);

        // var_40
        var_40_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x0);

        // var_42
        var_42_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x0);

        // reserve var_20 offsets, sub structure
        var_20_offset = buf->get_position();
        var_20_offset_table_pos = buf->get_position();
        buf->write_repeat(0x00, VAR_20_OFFSET_TABLE_SIZE * 4);

        // var_20_v2_connection
        var_20_v2_connection_offset = buf->get_position();
        std::wstring connection_utf16 = get_utf16(connection);
        int connection_len = connection_utf16.size() * sizeof(wchar_t);
        buf->write_u32(connection_len);
        buf->write_wstring(connection_utf16);

        // var_20_v3_ip
        var_20_v3_ip_offset = buf->get_position();
        std::wstring ip_utf16 = get_utf16(ip);
        int ip_len = ip_utf16.size() * sizeof(wchar_t);
        buf->write_u32(ip_len);
        buf->write_wstring(ip_utf16);

        // var_20_v4
        var_20_v4_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x2B0A);

        // var_20_v5
        var_20_v5_offset = buf->get_position();
        std::wstring var_20_v5_utf16 = get_utf16(var_20_v5);
        int var_20_v5_len = var_20_v5_utf16.size() * sizeof(wchar_t);
        buf->write_u32(var_20_v5_len);
        buf->write_wstring(var_20_v5_utf16);

        // var_20_v6
        var_20_v6_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x14924512);

        // var_20_v7
        var_20_v7_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x14924512);

        // var_20_v8
        var_20_v8_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x5);

        // var_20_v9
        var_20_v9_offset = buf->get_position();
        int var_20_v9_len = (var_20_v9.size() + 1);
        buf->write_u32(var_20_v9_len);
        buf->write_string(var_20_v9);

        // var_20_v10
        var_20_v10_offset = buf->get_position();
        buf->write_u32(0x2);
        buf->write_u16(0x0);

        // var_7
        var_7_offset = buf->get_position();
        buf->write_u32(0x0);

        // var_60
        var_60_offset = buf->get_position();
        buf->write_u32(var_60_size);
        buf->write_data(var_60, var_60_size);

        // var_14_tcls_client_exe
        var_14_tcls_client_exe_offset = buf->get_position();
        std::wstring tcls_client_exe_utf16 = get_utf16(tcls_client_exe);
        int tcls_client_exe_len = tcls_client_exe_utf16.size() * sizeof(wchar_t);
        buf->write_u32(tcls_client_exe_len);
        buf->write_wstring(tcls_client_exe_utf16);

        // var_41
        var_41_offset = buf->get_position();
        buf->write_u32(0x4);
        buf->write_u32(0x0);

        // var_15
        var_15_offset = buf->get_position();
        std::wstring var_15_utf16 = get_utf16(var_15);
        int unk_22_len = var_15_utf16.size() * sizeof(wchar_t);
        buf->write_u32(unk_22_len);
        buf->write_wstring(var_15_utf16);

        // var_16
        var_16_offset = buf->get_position();
        buf->write_u32(0x2);
        buf->write_u16(0x0);

        // write v20 offset table
        buf->seek(var_20_offset_table_pos);
        buf->write_u32(var_20_offset + 4); // points into offset table to connection offset entry
        buf->write_u32(var_20_v2_connection_offset);
        buf->write_u32(var_20_v3_ip_offset);
        buf->write_u32(var_20_v4_offset);
        buf->write_u32(var_20_v5_offset);
        buf->write_u32(var_20_v6_offset);
        buf->write_u32(var_20_v7_offset);
        buf->write_u32(var_20_v8_offset);
        buf->write_u32(var_20_v9_offset);
        buf->write_u32(var_20_v10_offset);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);

        // write offset table
        buf->seek(var_offset_table_pos);
        buf->write_u32(OFFSET_TABLE_SIZE);
        buf->write_u32(var_1_offset);
        buf->write_u32(var_2_offset);
        buf->write_u32(var_3_offset);
        buf->write_u32(var_4_offset);
        buf->write_u32(var_5_aes_key_offset);
        buf->write_u32(var_6_offset);
        buf->write_u32(var_7_offset);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);

        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(var_14_tcls_client_exe_offset);
        buf->write_u32(var_15_offset);
        buf->write_u32(var_16_offset);
        buf->write_u32(var_17_offset);
        buf->write_u32(var_18_offset);
        buf->write_u32(var_19_offset);
        buf->write_u32(var_20_offset);

        buf->write_u32(var_21_offset);
        buf->write_u32(var_22_offset);
        buf->write_u32(var_23_offset);
        buf->write_u32(var_24_offset);
        buf->write_u32(var_25_offset);
        buf->write_u32(var_26_offset);
        buf->write_u32(var_27_offset);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);

        buf->write_u32(0x0);
        buf->write_u32(var_32_offset);
        buf->write_u32(var_33_offset);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(var_36_offset);
        buf->write_u32(var_37_offset);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(var_40_offset);

        buf->write_u32(var_41_offset);
        buf->write_u32(var_42_offset);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);

        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(var_60_offset);

        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);
        buf->write_u32(0x0);

        buf->get_data(0, buf->get_length(), p_dst, p_dst_len);
        return buf->get_length();
    }

    void map(DWORD p_process_id) {
        if (p_process_id == 0) {
            p_process_id = _getpid();
        }
        std::string mem_name = "TCLS_SHAREDMEMEMORY" + std::to_string(p_process_id);
        size_t mem_size = 0xF001F;
        uint8_t *mapped_memory = map_memory(mem_name, mem_size);
        size_t written = write(mapped_memory, mem_size);
        show(mapped_memory, written);
    }

};

#endif //MHO_LAUNCHER_TENPROXY_TCLS_SHAREDMEMEMORY_H