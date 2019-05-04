#pragma once

#include <string>


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

    explicit TCPConnection(int descriptor);

    TCPConnection(const TCPConnection &) = delete;

    TCPConnection(TCPConnection &&) noexcept;

    TCPConnection &operator=(const TCPConnection &) = delete;

    TCPConnection &operator=(TCPConnection &&) noexcept;

    ~TCPConnection();

private:
    int conn_fd_{-1};
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
