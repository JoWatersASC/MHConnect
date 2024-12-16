#include "../include/client.h"
#include "../include/chat_room.h"

using namespace osf;

void Connection::send_text(const packet p) {
    send_tp.add([this, &p]() {
        std::unique_lock<std::mutex> lock(send_mtx); 
        send_pckt(m_fd, p); 
    });
}
void Connection::send_audio(packet p) {
	send_tp.add([this, p = std::move(p)]() {
        std::unique_lock<std::mutex> lock(send_mtx); 
        send_pckt(m_fd, std::move(p)); 
    });
}


void Connection::broadcast(const packet& p) {
    std::unique_lock<std::mutex> lock(send_mtx);
    room.broadcast(p, id);
}

void Connection::recv_text() {
    std::unique_lock<std::mutex> lock(recv_mtx);
    packet p;

    ssize_t bytes = recv_pckt(m_fd, p);
    p.id = id;

    if (bytes <= 0) {
#ifdef _WIN32
        closesocket(m_fd);
#else
        close(m_fd);
#endif
    } 
    else {
        send_tp.add([p = std::move(p), this]() { broadcast(std::move(p)); });
        recv_tp.add([this]() { recv_text(); });
    }
}