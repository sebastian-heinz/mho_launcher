#ifndef MHO_LAUNCHER_AG_BUFFER_H
#define MHO_LAUNCHER_AG_BUFFER_H

#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

class AgBuffer {

public:
    enum class Endianness {
        Big, Small
    };

    inline static Endianness get_system_endianness() {
        const int value{0x01};
        const void *address{static_cast<const void *>(&value)};
        const unsigned char *least_significant_address{static_cast<const unsigned char *>(address)};

        return (*least_significant_address == 0x01) ? Endianness::Small : Endianness::Big;
    }

private:
    inline static Endianness const system_endianness = get_system_endianness();

public:
    uint16_t swap_endianness(uint16_t x, Endianness p_endianness) {
        if (p_endianness != system_endianness) {
            return (x >> 8) | (x << 8);
        }
        return x;
    }

    uint32_t swap_endianness(uint32_t x, Endianness p_endianness) {
        if (p_endianness != system_endianness) {
            x = (x >> 16) | (x << 16);
            return ((x & 0xFF00FF00) >> 8) | ((x & 0x00FF00FF) << 8);
        }
        return x;
    }

    uint64_t swap_endianness(uint64_t x, Endianness p_endianness) {
        if (p_endianness != system_endianness) {
            x = (x >> 32) | (x << 32);
            x = ((x & 0xFFFF0000FFFF0000) >> 16) | ((x & 0x0000FFFF0000FFFF) << 16);
            return ((x & 0xFF00FF00FF00FF00) >> 8) | ((x & 0x00FF00FF00FF00FF) << 8);
        }
        return x;
    }

    void swap_endianness(uint8_t *p_data, size_t p_data_length, Endianness p_endianness) {
        if (p_endianness != system_endianness) {
            uint8_t *data_end = p_data + p_data_length - 1;
            while (p_data < data_end) {
                uint8_t temp = *p_data;
                *p_data = *data_end;
                *data_end = temp;
                p_data++;
                data_end--;
            }
        }
    }

private:
    const size_t GROW_SIZE = 1024;

    size_t position;
    size_t virtual_length;
    size_t real_length;
    uint8_t *data;
    Endianness endianness;

    void grow(size_t p_length) {
        size_t new_real_length = real_length + p_length + GROW_SIZE;
        void *new_loc = realloc(data, new_real_length);
        if (!new_loc) {
            // TODO error?
            return;
        }
        data = (uint8_t *) new_loc;
        real_length = new_real_length;
    }


public:
    void write_repeat(uint8_t p_repeat, size_t p_length) {
        for (int i = 0; i < p_length; i++) {
            write_u8(p_repeat);
        }
    }

    void write_data(void *p_src, size_t p_length) {
        if (p_length <= 0) {
            return;
        }
        if (position + p_length > real_length) {
            grow(p_length);
        }
        memcpy(&data[position], p_src, p_length);
        position += p_length;
        if (position > virtual_length) {
            virtual_length = position;
        }
    }

    void write_string(const std::string &p_val) {
        for (int i = 0; i < p_val.size(); i++) {
            write_u8(p_val[i]);
        }
        write_u8(0);
    }

    std::string read_string() {
        std::vector<char> buf;

        while (true) {
            uint8_t c = read_u8();
            if (c == 0) {
                break;
            }
            buf.push_back((char) c);
        }

        return std::string(buf.begin(), buf.end());
    }

    void write_wstring(const std::wstring &p_val) {
        for (int i = 0; i < p_val.size(); i++) {
            write_wchar(p_val[i]);
        }
    }

    void write_wchar(wchar_t wchar) {
        size_t wchar_size = sizeof(wchar_t);
        if (wchar_size == 2) {
            wchar = swap_endianness((uint16_t) wchar, endianness);
            write_u16(wchar);
        } else {
            // not supported
        }
    }

    uint8_t read_u8() {
        if (position >= virtual_length) {
            // end of file
            return 0;
        }
        uint8_t value = data[position];
        position++;
        return value;
    }

    uint16_t read_u16() {
        uint16_t value = (uint16_t) (read_u8() | read_u8() << 8);
        value = swap_endianness(value, endianness);
        return value;
    }

    uint32_t read_u32() {
        uint32_t value = (uint32_t) (read_u8() | (read_u8() << 8) | (read_u8() << 16) | (read_u8() << 24));
        value = swap_endianness(value, endianness);
        return value;
    }

    void write_u8(uint8_t p_val) {
        int val_length = 1;
        if (position + val_length > real_length) {
            grow(val_length);
        }
        data[position] = p_val;
        position += val_length;
        if (position > virtual_length) {
            virtual_length = position;
        }
    }

    void write_u16(uint16_t p_val) {
        p_val = swap_endianness(p_val, endianness);
        write_u8(p_val);
        write_u8(p_val >> 8);
    }

    void write_u32(uint32_t p_val) {
        p_val = swap_endianness(p_val, endianness);
        write_u8(p_val);
        write_u8(p_val >> 8);
        write_u8(p_val >> 16);
        write_u8(p_val >> 24);
    }

    void read_data(void *p_dst, size_t p_length) {
        if (p_dst == nullptr) {
            return;
        }
        if (p_length > virtual_length) {
            return;
        }
        if (position + p_length > virtual_length) {
            return;
        }
        memcpy(p_dst, &data[position], p_length);
        position += p_length;
    }

    void
    get_data(size_t p_src_offset, size_t p_src_count, void *p_dst, size_t p_dst_length) const {
        if (p_dst == nullptr) {
            return;
        }
        if (p_src_count > virtual_length) {
            return;
        }
        if (p_src_offset + p_src_count > virtual_length) {
            return;
        }
        if (p_dst_length < p_src_count) {
            return;
        }
        memcpy(p_dst, &data[p_src_offset], p_src_count);
    }

    void get_all_data(void *p_dst, size_t p_dst_length) const {
        if (p_dst_length <= 0) {
            return;
        }
        if (p_dst_length < virtual_length) {
            return;
        }
        if (p_dst == nullptr) {
            return;
        }
        memcpy(p_dst, &data[0], virtual_length);
    }

    void seek(size_t p_position) {
        if (p_position > virtual_length) {
            position = virtual_length;
            return;
        }
        position = p_position;
    }

    size_t get_position() const {
        return position;
    }

    size_t get_length() const {
        return virtual_length;
    }

    void set_length(size_t p_length) {
        if (p_length > real_length) {
            grow(p_length - real_length);
        }
        virtual_length = p_length;
    }

    void set_endianness(Endianness p_endianness) {
        endianness = p_endianness;
    }

    Endianness get_endianness() {
        return endianness;
    }

    size_t get_available() const {
        return virtual_length - position;
    }

    AgBuffer *slice(size_t p_length) {
        AgBuffer *buffer = new AgBuffer(&data[position], p_length);
        return buffer;
    }

    AgBuffer(void *p_data, size_t p_length) {
        real_length = p_length;
        data = (uint8_t *) malloc(real_length);
        position = 0;
        virtual_length = 0;
        endianness = Endianness::Small;
        write_data(p_data, p_length);
    }

    AgBuffer() {
        real_length = GROW_SIZE;
        data = (uint8_t *) malloc(real_length);
        position = 0;
        virtual_length = 0;
        endianness = Endianness::Small;
    }

    ~AgBuffer() {
        free(data);
    }

    uint8_t *get_ptr() {
        return data;
    }

};

#endif