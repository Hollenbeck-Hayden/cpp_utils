#pragma once

#include "BasicHandle.h"
#include <stdio.h>

class FileHandle : public BasicHandle {
public:
    FileHandle();
    FileHandle(const std::string& filename, OpenMode mode);

    virtual ~FileHandle();

    void open(const std::string& filename, OpenMode mode);
    virtual bool good() const override;
    virtual void close() override;
    
protected:
    FILE* file_;

    void _write(const uint8_t* buffer, size_t N);
    void _read(uint8_t* buffer, size_t N);
};

using FileReader = BinaryReaderTemplate<FileHandle>;
using FileWriter = BinaryWriterTemplate<FileHandle>;
using FileReaderWriter = BinaryReaderWriterTemplate<FileHandle>;
