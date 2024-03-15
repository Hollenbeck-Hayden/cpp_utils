#include "CppUtils/io/DeviceHandle.h"

#include "unistd.h"
#include "fcntl.h"

DeviceHandle::DeviceHandle()
    : BasicHandle(), fd_(-1)
{}

DeviceHandle::DeviceHandle(const std::string& filename, OpenMode mode)
    : DeviceHandle()
{
    open(filename, mode);
}

DeviceHandle::~DeviceHandle() {
    close();
}

void DeviceHandle::open(const std::string& filename, OpenMode mode) {
    mode_t umask = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    switch (mode) {
        case OpenMode::Append:
            fd_ = ::open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, umask);
            break;
        case OpenMode::Truncate:
            fd_ = ::open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, umask);
            break;
        case OpenMode::Read:
            fd_ = ::open(filename.c_str(), O_RDONLY, umask);
            break;
        case OpenMode::ReadWrite:
            fd_ = ::open(filename.c_str(), O_RDWR | O_CREAT, umask);
            break;
    }
}

bool DeviceHandle::good() const {
    return fd_ >= 0;
}

void DeviceHandle::close() {
    if (good()) {
        ::close(fd_);
        fd_ = -1;
    }
}

void DeviceHandle::_write(const uint8_t* buffer, size_t N) {
    int n_write = ::write(fd_, buffer, N);
    if (n_write != N)
        throw std::runtime_error("Write failure");
}


void DeviceHandle::_read(uint8_t* buffer, size_t N) {
    int n_read = ::read(fd_, buffer, N);
    if (n_read != N)
        throw std::runtime_error("Read failure");
}
