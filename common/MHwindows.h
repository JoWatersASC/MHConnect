#pragma once

#define _WINSOCKAPI_
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <BaseTsd.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define ssize_t SSIZE_T
#define send(sfd, data, size, flags) send(sfd, (const char *)data, size, flags)
#define recv(sfd, data, size, flags) recv(sfd, (char *)data, size, flags)
#define socket(v, w, x) socket(v, w, IPPROTO_TCP)


// #define accept(v, w, x) accept(v, w, (int *)x)
#define id_t int
#define socklen_t int
#define socket_t SOCKET


static WSADATA wsaData;
static int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

inline const int winsock_initialize() {
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return 1;  // Exit the program or handle the error appropriately
    }
    std::cout << "Winsock initialized successfully!" << std::endl;
    return 0;
}