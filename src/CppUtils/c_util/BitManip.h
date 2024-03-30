#pragma once

#include "CUtil.h"

#include <array>


enum class Endianness {
    Little,
    Big,
};

template <Endianness endian, size_t N_bytes>
class ByteArray {
public:
    BitArray(uint8_t* data)
        : data_(data)
    {}

    uint8_t& operator[](size_t i) {
        return data_[index(i)];
    }

    constexpr uint8_t operator[](size_t i) const {
        return data_[index(i)];
    }

    constexpr static size_t size() { return N_bytes; }

private:
    constexpr size_t index(size_t i) const {
        switch(endian) {
            case Endianness::Little:
                return i;
            case Endianness::Big:
                return (N_bytes - 1) - i;
        }
    }

    uint8_t* data_;
};

constexpr static size_t n_bits_per_byte = 8;

template <typename T>
constexpr size_t bit_sizeof() {
    return sizeof(T) * n_bits_per_byte;
}

constexpr size_t containing_size_bytes(size_t N_bits) {
    return (N_bits / n_bits_per_byte) + static_cast<size_t>((N_bits % n_bits_per_byte) > 0);
}

template <size_t left, size_t middle, size_t right, typename T>
constexpr T interval_mask() {
    static_assert(left + middle + right == sizeof(T));
    if (middle == sizeof(T)) return ~T{0};
    return ((T{0b1} << middle) - 1) << right;
}




template <Endianness endian, size_t N_bits>
class BitArray {
public:

    template <Endianness endian, typename T>
    static BitArray<endian, bit_sizeof(T)> make_array(T* data) {
        return BitArray<endian, bit_sizeof(T)>((uint8_t*) data);
    }

    BitArray(uint8_t* data)
        : data_(data)
    {}
    
    constexpr bool operator[](size_t i) const {
        return get_bit(i);
    }

    constexpr bool get_bit(size_t i) const {
        return (data_[byte_index(i)] & bit_flag(i)) != 0;
    }

    void set_bit_on(size_t i) {
        data_[byte_index(i)] |= bit_flag(i);
    }

    void set_bit_off(size_t i) {
        data_[byte_index(i)] &= ~bit_flag(i);
    }

    void set_bit(size_t i, bool value) {
        if (value)  set_bit_on (i);
        else        set_bit_off(i);
    }

    template <typename T>
    T convert() const {
        static_assert(sizeof(T) == data_.size());
        T result = 0;
        for (size_t i = 0; i+1 < data_.size(); i++) {
            result |= static_cast<T>(data_[data_.size() - 1 - i]);
            result << n_bits_per_byte;
        }
        if (data_.size() > 0) {
            constexpr size_t buffer_bits = n_bits_per_byte * data_.size() - N_bits;
            constexpr uint8_t mask = interval_mask<0, buffer_bits, n_bits_per_byte - buffer_bits, uint8_t>();
            if constexpr (std::is_signed_v<T> && get_bit(N_bits - 1)) {
                result |= static_cast<T>(mask | data_[0]);
            } else {
                result |= static_cast<T>(~mask & data_[0]);
            }
        }
        return result;
    }

    void twos_compliment() {
        if (data_.size() == 0)
            return;

        data_[0] = (~data_[0]) + 1;
        bool carry_bit = data_[0] == 0;
        for (size_t i = 1; i < data_.size(); i++) {
            data_[i] = ~data_[i];
            if (carry_bit) {
                carry_bit = (++data_[i]) == 0;
            }
        }
    }

private:
    constexpr size_t byte_index(size_t i) const {
        return i/n_bits_per_byte;
    }

    constexpr size_t bit_flag(size_t i) const {
        return 0b1 << (i % n_bits_per_byte);
    }

    ByteArray<endian, containing_size_bytes(N_bits)> data_;
};





template <typename T>
uint8_t byte_cast(T value) {
    return static_cast<uint8_t>(value);
}

template <typename T>
using byte_array_type = std::array<uint8_t, sizeof(T)>;


template <Endianness endian, typename T>
void write_bitwise_array(uint8_t* buffer, T value) {
    EndianArrayBuilder<endian, sizeof(T)> builder(buffer);
    for (size_t i = 0; i < sizeof(T); i++) {
        builder[i] = byte_cast(value);
        value >> 8;
    }
    return result;
}

template <Endianness endian, typename T>
T read_bitwise_array(uint8_t* buffer) {
    T result;
    EndianArrayBuilder<endian, sizeof(T)> builder(buffer);
    for (size_t i = 0; i < sizeof(T); i++) {
        if (i > 0) result << 8;
        result |= builder[i];
    }
    return result;
}


