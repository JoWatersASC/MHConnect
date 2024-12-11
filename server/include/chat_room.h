#pragma once

#include <unordered_map>

#include "client.h"

namespace osf 
{

class ChatRoom {
public:
    ChatRoom() = delete;
    ChatRoom(const ChatRoom&) = delete;
    ChatRoom(ChatRoom&&) = default;

    ChatRoom(ThreadPool& r_tp, ThreadPool& s_tp, size_t num_clients = 4) 
    : recv_tp(r_tp), send_tp(s_tp) 
    {
        srand(time(0));
        m_clients.reserve(num_clients);
    }

    void broadcast(const packet&, const id_t&);
    void broadcast(std::string);
    void add_client(int, sockaddr_in);


private:
    std::vector<Connection> m_clients;
    std::unordered_map<id_t, Connection&> client_ids;
    ThreadPool& recv_tp;
    ThreadPool& send_tp;
    std::mutex m_mtx;

};

}