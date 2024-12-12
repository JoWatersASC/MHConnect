#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <BaseTsd.h>

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
static int result = WSAStartup(MAKEWORD(2, 2), &wsaData);  // Request Winsock 2.2

// If WSAStartup succeeds, continue with socket programming


// #define close(x) closesocket(x)