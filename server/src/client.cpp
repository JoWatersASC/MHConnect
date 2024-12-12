#include "../include/client.h"
#include "../include/chat_room.h"

using namespace osf;

void Connection::send_text(const packet p) {
    send_tp.add([this, &p]() { send_pckt(m_fd, p); });
}

void Connection::broadcast(const packet& p) {
    std::unique_lock<std::mutex> lock(send_mtx);
    room.broadcast(p, id);
}

void Connection::recv_text() {
    packet p;

    ssize_t bytes = recv_pckt(m_fd, p);
    p.id = id;

    if (bytes <= 0) {
#ifdef WIN32
        closesocket(m_fd);
#else
        close(m_fd);
#endif
    } 
    else {
        send_tp.add([p, this]() { broadcast(p); });
        recv_tp.add([this]() { recv_text(); });
    }
}