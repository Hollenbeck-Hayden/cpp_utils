#pragma once

#include "BasicHandle.h"
#include <stdexcept>

class DeviceHandle : public BasicHandle {
public:
    DeviceHandle();
    DeviceHandle(const std::string& filename, OpenMode mode);

    virtual ~DeviceHandle();

    void open(const std::string& filename, OpenMode mode);
    virtual bool good() const override;
    virtual void close() override;

protected:
    int fd_;

    void _write(const uint8_t* buffer, size_t N);
    void _read(uint8_t* buffer, size_t N);
};

using DeviceWriter = BinaryWriterTemplate<DeviceHandle>;
using DeviceReader = BinaryReaderTemplate<DeviceHandle>;
using DeviceReaderWriter = BinaryReaderWriterTemplate<DeviceHandle>;
