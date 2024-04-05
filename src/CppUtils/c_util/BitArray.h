#pragma once

#include "ByteArray.h"

constexpr size_t containing_size_bytes(size_t N_bits) {
    return (N_bits / n_bits_per_byte) + static_cast<size_t>((N_bits % n_bits_per_byte) > 0);
}

/*
 * Handles an N-bit data type. Provides bitwise access, as well as
 * simple operations on the type as an N-bit unsigned / signed integer.
 *
 * The underlying data is stored in a buffer which is endian independent.
 * Bits in the BitArray are indexed in the canonical bit-string format
 * as though they were a fundamental integer type, e.g.
 *
 * 12-bit array:
 * x x x x 0 1 0 1     1 1 0 1 1 0 1 0 0 1
 *        11  <- 8     7              <- 0
 *
 * Padding bits are reserved and shouldn't be used. Data is always
 * assumed to be right-justified (padding bits are to the left).
 */
template <Endianness endian, size_t N_bits>
class BitArray {
public:
    using Self = BitArray<endian, N_bits>;

    constexpr static size_t N_bytes = containing_size_bytes(N_bits);
    constexpr static size_t N_padding = n_bits_per_byte * N_bytes - N_bits;
    constexpr static uint8_t padding_mask = interval_mask<0, N_padding, n_bits_per_byte - N_padding, uint8_t>();

    BitArray(uint8_t* data)
        : data_(data)
    {}

    BitArray(const ByteArray<endian, N_bytes>& data)
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

    /*
     * Convert the bit array to a fundamental integral type represented by the
     * same number of bits.
     */
    template <typename T>
    T convert() const {
        static_assert(sizeof(T) >= data_.size());
        T result = 0;
        if (!data_.empty()) {
            if (std::is_signed_v<T> && get_bit(N_bits - 1)) {
                constexpr size_t N_msb_bits = n_bits_per_byte - N_padding;
                result = interval_mask<0, bit_sizeof<T>() - N_msb_bits, N_msb_bits, T>();
                result |= static_cast<T>(padding_mask | data_.msb());
            } else {
                result |= static_cast<T>(~padding_mask & data_.msb());
            }
        }
        for (size_t i = 1; i < data_.size(); i++) {
            result = result << n_bits_per_byte;
            result |= static_cast<T>(data_[data_.size() - 1 - i]);
        }
        
        return result;
    }

    void twos_compliment() {
        if (data_.size() == 0)
            return;

        data_.lsb() = (~data_.lsb()) + 1;
        bool carry_bit = data_.lsb() == 0;
        for (size_t i = 1; i < data_.size(); i++) {
            data_[i] = ~data_[i];
            if (carry_bit) {
                carry_bit = (++data_[i]) == 0;
            }
        }
    }

    ByteArray<endian, N_bytes>& bytes() { return data_; }
    const ByteArray<endian, N_bytes>& bytes() const { return data_; }

    friend std::ostream& operator<<(std::ostream& out, const Self& self) {
        std::ios_base::fmtflags flags(out.flags());

        out << std::noboolalpha;
        for (size_t i = 0; i < N_bits; i++) {
            out << self.get_bit(N_bits-1-i);
        }
        
        out.flags(flags);
        return out;
    }

    constexpr static bool empty() { return N_bits == 0; }

private:
    constexpr size_t byte_index(size_t i) const {
        return i/n_bits_per_byte;
    }

    constexpr size_t bit_flag(size_t i) const {
        return 0b1 << (i % n_bits_per_byte);
    }

    ByteArray<endian, N_bytes> data_;
};

template <Endianness endian, size_t N, typename T, typename U>
BitArray<endian, N> make_bit_array(ArrayView<T, sizeof(U)/sizeof(T)> buffer, U value) {
    static_assert(buffer.size() * sizeof(T) == sizeof(U));
    return BitArray<endian, N>(make_byte_array<endian>(buffer, value));
}

/*
 * Left-justifies a big-endian bit array. This invalidates the bit array.
 */
template <size_t N_bits>
void left_justify(BitArray<Endianness::Big, N_bits>& data) {
    if (data.empty()) return;

    for (size_t i = 0; i+1 < data.N_bytes; i++) {
        size_t index = data.N_bytes - i - 1;
        data.bytes()[index] = left_justify<n_bits_per_byte - data.N_padding>(data.bytes()[index]) 
                              | right_justify<data.N_padding>(data.bytes()[index-1]);
    }
    data.bytes().lsb() = left_justify<n_bits_per_byte - data.N_padding>(data.bytes().lsb());
}

/*
 * Right-justifies a data array and returns it as a big-endian bit array.
 */
template <size_t N_bits>
BitArray<Endianness::Big, N_bits> right_justify(ArrayView<uint8_t, containing_size_bytes(N_bits)> buffer) {
    if constexpr (N_bits == 0) return BitArray<Endianness::Big,N_bits>(nullptr);

    BitArray<Endianness::Big, N_bits> data(buffer);

    for (size_t i = 0; i+1 < data.N_bytes; i++) {
        data.bytes()[i] = right_justify<n_bits_per_byte - data.N_padding>(data.bytes()[i])
                          | left_justify<data.N_padding>(data.bytes()[i+1]);
    }
    data.bytes().msb() = right_justify<n_bits_per_byte - data.N_padding>(data.bytes().msb());
    return data;
}
