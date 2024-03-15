#pragma once

#include "CppUtils/io/BasicHandle.h"

#include <sys/socket.h>
#include <netinet/in.h>

class SocketHandle : public BasicHandle {

    SocketHandle();
    virtual ~SocketHandle();

    virtual bool good() const override;
    virtual void close() override;

    void listen(int port);
    void connect(const std::string& hostname, int port);
    void accept(const SocketHandle& server);

protected:
    virtual void _write(const uint8_t* buffer, size_t N);
    virtual void _read(uint8_t* buffer, size_t N);

    int socket_fd_;
    struct sockaddr_in address_;
};

using Socket = BinaryReaderWriterTemplate<SocketHandle>;
