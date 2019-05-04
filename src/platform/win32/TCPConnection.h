#pragma once

#include <string>
#include <WinSock2.h>


class TCPConnection {
public:
    void write(const void *data, size_t size);

    template<typename T>
    void write(const T &object);

    void read(void *data, size_t size);

    template<typename T>
    void read(T &object);

    template<typename T>
    T read();

    TCPConnection(const std::string &address, const std::string &port);

    explicit TCPConnection(SOCKET descriptor);

    TCPConnection(const TCPConnection &) = delete;

    TCPConnection(TCPConnection &&) noexcept;

    TCPConnection &operator=(const TCPConnection &) = delete;

    TCPConnection &operator=(TCPConnection &&) noexcept;

    ~TCPConnection();

private:
    SOCKET conn_fd_{INVALID_SOCKET};
};

////////////////////////////////////////////////////////////////

template<typename T>
void TCPConnection::write(const T &object) {
    write(&object, sizeof(object));
}

template<typename T>
void TCPConnection::read(T &object) {
    read(&object, sizeof(object));
}

template<typename T>
T TCPConnection::read() {
    T obj;
    read(obj);
    return obj;
}
