#pragma once

#include "TCPConnection.h"


class TCPListener {
public:
    explicit TCPListener(const std::string &port);

    TCPConnection listen();

    TCPListener(const TCPListener &) = delete;

    TCPListener(TCPListener &&) = default;

    TCPListener &operator=(const TCPListener &) = delete;

    TCPListener &operator=(TCPListener &&) = default;

    ~TCPListener();

private:
    SOCKET listen_fd_{INVALID_SOCKET};
};
