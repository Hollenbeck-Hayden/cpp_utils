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

DeviceHandle::DeviceHandle(const std::string& filename, int mode)
    : DeviceHandle()
{
    open_raw(filename, mode);
}

DeviceHandle::~DeviceHandle() {
    close();
}

void DeviceHandle::open(const std::string& filename, OpenMode mode) {
    switch (mode) {
        case OpenMode::Append:
            open_raw(filename, O_WRONLY | O_CREAT | O_APPEND);
            break;
        case OpenMode::Truncate:
            open_raw(filename, O_WRONLY | O_CREAT | O_TRUNC);
            break;
        case OpenMode::Read:
            open_raw(filename, O_RDONLY);
            break;
        case OpenMode::ReadWrite:
            open_raw(filename, O_RDWR | O_CREAT);
            break;
    }
}

void DeviceHandle::open_raw(const std::string& filename, int mode) {
    const mode_t umask = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    fd_ = ::open(filename.c_str(), mode, umask);
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
    size_t n_write = ::write(fd_, buffer, N);
    if (n_write != N)
        throw std::runtime_error("Write failure");
}


void DeviceHandle::_read(uint8_t* buffer, size_t N) {
    size_t n_read = ::read(fd_, buffer, N);
    if (n_read != N)
        throw std::runtime_error("Read failure");
}
