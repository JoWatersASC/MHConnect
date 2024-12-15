#pragma once

// #include "./connection.h"
#include "../../common/threadpool.h"
#include<memory>

namespace osf
{

class ChatRoom;

class Connection {
public:
    Connection() = delete;
    Connection(ThreadPool& r_tp, ThreadPool& s_tp, ChatRoom& _room, socket_t _fd, sockaddr_in _addr, ssize_t _id)
    : recv_tp(r_tp), send_tp(s_tp), room(_room), m_fd(_fd), m_addr(_addr), id(_id) {}

    Connection(Connection&& other) noexcept 
    : recv_tp(other.recv_tp), 
      send_tp(other.send_tp), 
      room(other.room), 
      id(other.id), 
      m_fd(other.m_fd), 
      m_addr(other.m_addr),
      m_err(other.m_err),
      connected(other.connected.load()) {}

    void broadcast(const packet& p);
    void send_text(const packet p);
	void send_audio(const packet p);
	
    void recv_text();

    std::string getIp() {
        return inet_ntoa(m_addr.sin_addr);
    }

    friend bool operator==(const Connection& a, const Connection& b) {
        return a.id == b.id;
    }

    const ssize_t getId() const { return id; }

    friend std::ostream& operator<<(std::ostream& out, const Connection& _client) {
        out << "Connection ID: " << std::to_string(_client.id);
        out << " Connection Ip: " << std::string(inet_ntoa(_client.m_addr.sin_addr));
        return out;
    }

private:
    // tcp::TextConnection tc;
    // udp::AudioConnection ac;
    // udp::VideoConnection vc;

    ThreadPool& recv_tp;
    ThreadPool& send_tp;
    ChatRoom& room;

    std::mutex send_mtx;
    std::mutex recv_mtx;

    const id_t id;
    const socket_t m_fd;
    const sockaddr_in m_addr;
    int m_err;
    std::atomic<bool> connected;
};

}