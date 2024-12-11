#pragma once

#include "../../common/common.h"

#include<memory>
#include<algorithm>
#include<atomic>

namespace osf
{

class Client {
public:
    Client();
    Client(int _fd, sockaddr_in& _addr) 
    : m_sock_fd(std::move(_fd)), m_addr(std::move(_addr)), connected(false) {}

    void start_connect();
    void start_recv();
    void start_send();

    void close();

private:
    int m_sock_fd;
    sockaddr_in m_addr;

    packet m_pckt;

    std::atomic<bool> connected;

};

inline sockaddr_in create_address(std::string ip, int port) {
    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &address.sin_addr.s_addr);

    return std::move(address);
}

}