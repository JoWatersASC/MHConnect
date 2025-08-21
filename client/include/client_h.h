#pragma once

#include "common.h"
#include "context.h"
#include "threadpool.h"

#include <atomic>

namespace osf
{

class Client {
public:
	Client();
	Client(int _fd, sockaddr_in& _addr, client_context &_ctx);

	bool try_connect();
	void start_recv();
	void start_send();
	void start_send(const std::string& msg);
	
	const packet poll_msgs() { return msg_queue.pop_front(); }
	const bool pollable() { return !msg_queue.empty(); }

	void close();

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
		virtual ~listener() {}
		virtual void onNotify(const packet&) = 0;
	};

	void addListener(listener* _listener) { listeners.push_back(_listener); }

private:
	std::vector<listener *> listeners;

	void notify() {
		if (!pollable())
			return;

		std::lock_guard<std::mutex> lock(recv_mtx);
		const packet p = msg_queue.pop_front();

		for (auto& listener : listeners) {
			ctx.add_recv_task( [&] { listener->onNotify(p); } );
		}
	}
};

inline sockaddr_in create_address(std::string ip, int port) {
	sockaddr_in address{};

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &address.sin_addr.s_addr);

	return address;
}

}
