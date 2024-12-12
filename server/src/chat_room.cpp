#include "../include/chat_room.h"

using namespace osf;

void ChatRoom::broadcast(const packet p, const id_t& source_id) {
    std::unique_lock<std::mutex> lock(m_mtx);

    std::cout << "FROM: " << source_id << std::endl;
    std::cout << "[MSG] " << p << std::endl;

    for(Connection& _client : m_clients) {
        if(!(_client.getId() == source_id)) {
            _client.send_text(p);
        }
    }
}
// void ChatRoom::broadcast(std::string msg) {
//     std::cerr << "String broadcast" << std::endl;
//     packet p;
//     p.type = PCKTYPE::TEXT;
//     p = msg;

//     broadcast(p, 0);
// }

void ChatRoom::add_client(int _fd, sockaddr_in _addr) {
    std::unique_lock<std::mutex> lock(m_mtx);

    id_t new_client_id;

    do {
        new_client_id = rand();
    } while(client_ids.count(new_client_id));
    
    m_clients.emplace_back(recv_tp, send_tp, *this, 
        std::move(_fd), std::move(_addr), new_client_id);

    std::cout << "New client connected: ip-" << inet_ntoa(_addr.sin_addr) << std::endl;

    client_ids.emplace(new_client_id, m_clients.back());

    packet id_pckt;
    id_pckt.type = PCKTYPE::TEXT;
    id_pckt.id = new_client_id;
    id_pckt = std::string("ID assigned: " + std::to_string(new_client_id));

    m_clients.back().send_text(id_pckt);
    recv_tp.add([&]() {
        m_clients.back().recv_text(); 
    });
}