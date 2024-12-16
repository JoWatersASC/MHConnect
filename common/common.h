#pragma once

#include<iostream>

#ifdef _WIN32

#include "MHwindows.h"

#elif defined(__linux__)
#include<netinet/in.h>
#include<unistd.h>
#include<signal.h>
#include<arpa/inet.h>

#define socket_t int
#endif

#include<utility>
#include<string>
#include<vector>
#include<queue>
#include<condition_variable>
#include<thread>
#include<mutex>
#include<fstream>
#include<algorithm>
#include<cstring>
#include<cctype>
#include<cstdint>


#define DEBUG 0

using Byte = unsigned char;
constexpr uint16_t PACK_LEN = 2048;  

enum class PCKTYPE : short { DEFAULT, TEXT, AUDIO, VIDEO };

struct packet {
    PCKTYPE type;
    uint32_t id;
    char data[PACK_LEN];

    packet() : id(0) {
        type = PCKTYPE::DEFAULT;
        memset(data, 0, PACK_LEN);
    }

    packet& operator=(const packet&) = default;

    packet& operator=(const std::string& _data) {
        memset(data, 0, sizeof(data));
        memcpy(data, _data.data(), sizeof(data) - 1);

        return *this;
    }
};

inline packet createPacket(const std::string& message) {
    packet p;
    p = message;
    return p;
}

inline ssize_t send_pckt(const socket_t socket_fd, const packet& p) {
    ssize_t bytes_sent = 0;
    ssize_t bytes;

    while (bytes_sent < sizeof(packet)) {
        bytes = send(socket_fd, (Byte*)&p + bytes_sent, sizeof(packet) - bytes_sent, 0);

        if (bytes <= 0) {
            std::cerr << "[SERVER] " << strerror(errno) << std::endl;
            break;
        }
        bytes_sent += bytes;
    }

    return bytes_sent;
}
inline ssize_t recv_pckt(const socket_t socket_fd, packet& p) {
    ssize_t bytes_received = 0;
    ssize_t bytes;

    while (bytes_received < sizeof(packet)) {
        bytes = recv(socket_fd, &p + bytes_received, sizeof(packet) - bytes_received, 0);

        if (bytes <= 0) {
            std::cerr << "[SERVER] " << strerror(errno) << std::endl;
            break;
        }
        bytes_received += bytes;
    }

    return bytes_received;
}

inline std::ostream& operator<<(std::ostream& out, const packet& p) {
    std::string _type;
    switch(p.type) {
        case PCKTYPE::DEFAULT:
            _type = "Default";
            break;

        case PCKTYPE::TEXT:
            _type = "Text";
            break;

        case PCKTYPE::AUDIO:
            _type = "Audio";
            break;

        case PCKTYPE::VIDEO:
            _type = "Video";
            break;

        default:
            _type = "Invalid";
            break;
    }

    out << p.data;
    return out;
}

inline bool operator==(const sockaddr_in& a, const sockaddr_in& b) {
    return a.sin_addr.s_addr == b.sin_addr.s_addr 
        && a.sin_family == b.sin_family
        && a.sin_port == b.sin_port;
}

namespace osf {
    inline uint16_t PORT = 16000;
}