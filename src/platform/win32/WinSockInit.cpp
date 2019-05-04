#include <winsock2.h>


namespace win_init_impl {
    static struct WinSockInit {
        WinSockInit() {
            WSADATA data;
            WSAStartup(MAKEWORD(2, 2), &data);
        }
    } WinSockInit;
}