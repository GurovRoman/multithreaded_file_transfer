#include <stdexcept>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "TCPConnection.h"


TCPConnection::TCPConnection(const std::string &address, const std::string &port) {
    addrinfo addr_hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
    addrinfo *resolved_addr = nullptr;
    int error = getaddrinfo(address.c_str(), port.c_str(), &addr_hints, &resolved_addr);
    if (error != 0) {
        throw std::runtime_error(gai_strerror(error));
    }

    conn_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    error = ::connect(conn_fd_, resolved_addr->ai_addr, resolved_addr->ai_addrlen);

    freeaddrinfo(resolved_addr);

    if (error == SOCKET_ERROR) {
        throw std::runtime_error("Connection failed");
    }
}

TCPConnection::TCPConnection(SOCKET descriptor) {
    conn_fd_ = descriptor;
}

TCPConnection::TCPConnection(TCPConnection &&other) noexcept {
    std::swap(conn_fd_, other.conn_fd_);
}

TCPConnection &TCPConnection::operator=(TCPConnection &&other) noexcept {
    if (conn_fd_ != INVALID_SOCKET) {
        closesocket(conn_fd_);
        conn_fd_ = INVALID_SOCKET;
    }
    std::swap(conn_fd_, other.conn_fd_);
    return *this;
}

TCPConnection::~TCPConnection() {
    if (conn_fd_ != INVALID_SOCKET) {
        closesocket(conn_fd_);
    }
}

void TCPConnection::write(const void *data, size_t size) {
    while (size > 0) {
        int written = ::send(conn_fd_, (const char *) data, size, 0);
        if (written < 0) {
            throw std::runtime_error("Connection closed");
        }
        size -= written;
        data = (const char *) data + written;
    }
}

void TCPConnection::read(void *data, size_t size) {
    while (size > 0) {
        int read = ::recv(conn_fd_, (char *) data, size, 0);
        if (read <= 0) {
            throw std::runtime_error("Connection closed");
        }
        size -= read;
        data = (char *) data + read;
    }
}
