#pragma once

#include "chat_room.h"

namespace osf
{
    
class Server {
public:
    Server(size_t _num_threads = 5) : recv_tp(_num_threads * 2 / 3), send_tp(_num_threads - (_num_threads * 2 / 3)),
    m_room(recv_tp, send_tp), running(false)
    {}
    Server(const Server&) = delete;

    void start();
    void accept_client();

private:
    int m_sock_fd;
    ThreadPool recv_tp;
    ThreadPool send_tp;
    
    ChatRoom m_room;
    std::atomic<bool> running;

};

}