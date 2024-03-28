#pragma once

#include <cstdint>
#include <string>

class BinaryReader {
public:
    template <typename T>
    void read(T& t) {
        read<T, 1>(&t);
    }

    template <typename T, size_t N>
    void read(T* buffer) {
        read<T>(buffer, N);
    }

    template <typename T>
    void read(T* buffer, size_t N) {
        this->read_impl((uint8_t*) buffer, sizeof(T) * N);
    }

    template <typename T>
    void read_buffer(T& buffer) {
        read(buffer.data(), buffer.size());
    }

    template <typename T>
    std::string read_string(T& buffer) {
        read_buffer(buffer);
        return std::string(buffer.data(), buffer.size());
    }

protected:
    virtual void read_impl(uint8_t* buffer, size_t N) = 0;
};


template <typename T>
class BinaryReaderTemplate : public T, public BinaryReader {
public:
    template <typename... Args>
    BinaryReaderTemplate(Args&&... args)
        : T(std::forward<Args>(args)...), BinaryReader()
    {}

    virtual ~BinaryReaderTemplate() {}

protected:
    virtual void read_impl(uint8_t* buffer, size_t N) override {
        T::_read(buffer, N);
    }
};


