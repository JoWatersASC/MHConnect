#pragma once

#include "common.h"
#include "threadpool.h"

#include <atomic>
#include <mutex>

class Interface;

namespace osf
{


struct client_context;

class Client {
public:
	Client();
	Client(int _fd, sockaddr_in& _addr, client_context &_ctx);

	const socket_t get_sock() const { return m_sock_fd; }
	bool try_connect();
	void start_recv();
	void start_send();
	void start_send(const std::string& msg);

	//GIT CHANGE, defined below 2 functions
	
	const packet poll_msgs() { return msg_queue.pop_front(); }
	const bool pollable() { return !msg_queue.empty(); }

	[[nodiscard]] const bool is_open() const { return connected; }
	void close();
	friend struct client_context;

private:
	socket_t m_sock_fd;
	sockaddr_in m_addr;
	client_context &ctx;
	std::atomic<bool> connected;
	tqueue<packet> msg_queue;

	std::mutex send_mtx;
	std::mutex recv_mtx;

public:
	struct listener {
		virtual ~listener() {
			client_->remListener(this);
		}
		virtual void onNotify(const packet&) = 0;

		Client *client_ = nullptr;
	};

	void addListener(listener *_listener) {
		listeners.push_back(_listener);
		_listener->client_ = this;
	}
	void remListener(listener *_listener) {
		auto &vec = listeners;
		vec.erase(std::remove(vec.begin(), vec.end(), _listener), vec.end());
	}

private:
	std::vector<listener*> listeners;

	void notify();
};

inline sockaddr_in create_address(std::string ip, int port) {
	sockaddr_in address{};

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &address.sin_addr.s_addr);

	return address;
}

}
