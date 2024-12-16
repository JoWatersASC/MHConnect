#pragma once

#include "../../common/common.h"
#include "threadpool.h"
#include "audio.h"

#include<memory>
#include<algorithm>
#include<atomic>

namespace osf
{

class Client {
public:
    Client();
    Client(int _fd, sockaddr_in& _addr, size_t _nthreads) 
    : m_sock_fd(_fd), m_addr(std::move(_addr)),
    recv_tp(_nthreads / 3), send_tp(_nthreads * 2 / 3),
    m_inaudio(m_sock_fd, send_tp), m_outaudio(m_sock_fd, recv_tp) {
        send_tp.start();
        recv_tp.start();
    }

    void start_connect();
    void start_recv();
    void start_send();

    void start_audio_in();
    void start_audio_out();
    
    void close();

private:
    int m_sock_fd;
    sockaddr_in m_addr;
    AudioStreamer m_inaudio;
    AudioReceiver m_outaudio;

    ThreadPool recv_tp;
    ThreadPool send_tp;

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