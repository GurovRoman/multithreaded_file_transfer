#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "TCPListener.h"


TCPListener::TCPListener(const std::string &port) {
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    int opt_val = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEPORT, &opt_val, sizeof(opt_val));

    sockaddr_in local_addr = {.sin_port = htons(atoi(port.c_str()))};
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_family = AF_INET;

    bind(listen_fd_, (struct sockaddr *) &local_addr, sizeof(local_addr));

    ::listen(listen_fd_, 100);
}

TCPConnection TCPListener::listen() {
    sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);

    int client_fd = accept(listen_fd_, (struct sockaddr *) &client_addr, &addrlen);

    return TCPConnection(client_fd);
}

TCPListener::~TCPListener() {
    if (listen_fd_ != -1) {
        close(listen_fd_);
    }
}
