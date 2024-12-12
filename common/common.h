#pragma once

#include<utility>
#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<condition_variable>
#include<thread>
#include<mutex>
#include<fstream>
#include<algorithm>

#include<cstring>
#include<netinet/in.h>
#include<unistd.h>
#include<signal.h>
#include<arpa/inet.h>

#define DEBUG 0

using Byte = unsigned char;
constexpr uint16_t PACK_LEN = 4096;

enum class PCKTYPE : short { DEFAULT, TEXT, AUDIO, VIDEO };

struct packet {
    PCKTYPE type = PCKTYPE::TEXT;
    uint32_t id = 0;
    char data[PACK_LEN];

    packet& operator=(const packet&) = default;

    packet& operator=(const std::string& _data) {
        memset(data, 0, sizeof(data));
        memcpy(data, _data.data(), sizeof(data));

        return *this;
    }
};

inline packet createPacket(const std::string& message) {
    packet p;
    p = message;
    return p;
}

inline ssize_t send_pckt(const int socket_fd, const packet& p) {
    return send(socket_fd, &p, sizeof(packet), 0);
}
inline ssize_t recv_pckt(const int socket_fd, packet& p) {
  return recv(socket_fd, &p, sizeof(p), 0);
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
    
    if(DEBUG)
        out << "Type: " << _type << ", ID: " << p.id << ", Data: ";

    out << p.data;
    return out;
}

inline bool operator==(const sockaddr_in& a, const sockaddr_in& b) {
    return a.sin_addr.s_addr == b.sin_addr.s_addr 
        && a.sin_family == b.sin_family
        && a.sin_port == b.sin_port;
}
