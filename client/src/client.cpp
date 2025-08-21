#include "client.h"
#include "context.h"

namespace osf
{

static auto YOU_STR = "[YOU] ";

Client::Client(int _fd, sockaddr_in& _addr, client_context &_ctx)
	: m_sock_fd(_fd), m_addr(_addr), ctx(_ctx) {}

bool Client::try_connect() {
	int res = ::connect(m_sock_fd, (sockaddr *)&m_addr, sizeof(m_addr));

	if(res == 0) {
		std::cout << "Connected to " << inet_ntoa(m_addr.sin_addr);
		std::cout << " on port " << ntohs(m_addr.sin_port) << std::endl;
		connected = true;

	} else {
		std::cout << "Connection failed: [ERROR] ";
		std::cout << strerror(errno) << std::endl;
		close();
		return false;
	}

	return true;
}

void Client::start_recv() {
	if(!connected) {
		std::cout << "Reception impossible, socket not connected" << std::endl;
		return;
	}

	packet out_pckt;
	ssize_t bytes;

	{
		std::lock_guard<std::mutex> lock(ctx.recv_mtx);
		bytes = recv_pckt(m_sock_fd, out_pckt);
	}

	if(bytes > 0 && connected) {
		msg_queue.enqueue(out_pckt);
		notify();
		ctx.add_recv_task([this] { start_recv(); });
	} else {
		if(bytes == 0) {
			std::cout << "Orderly shutdown from server" << std::endl;
			connected = false;
		}
		else {
			std::cout << "Reception failed: [ERROR] ";
			std::cout << strerror(errno) << std::endl;
		}
	}
}

void Client::start_send() {
	if(!connected) {
		std::cout << "Data transmission impossible, socket not connected" << std::endl;
		return;
	}

	std::string msg;
	std::cout << YOU_STR;

	while(std::getline(std::cin, msg) && connected) {
		start_send(msg);

		std::cout << YOU_STR;
	}

	close();
}
void Client::start_send(const std::string& _msg) {
	packet out_pckt{};
	out_pckt.type = PCKTYPE::TEXT;
	out_pckt = _msg;

	ssize_t bytes;

	{
		std::lock_guard<std::mutex> send_lock(send_mtx);
		bytes = send_pckt(m_sock_fd, out_pckt);
	}

	if(bytes < 0) {
		std::cout << "Packet send failed: [ERROR] ";
		std::cout << strerror(errno) << std::endl;
	}
}

void Client::notify() {
	if (!pollable())
		return;

	std::lock_guard<std::mutex> lock(recv_mtx);
	const packet p = msg_queue.pop_front();

	for (auto& listener : listeners) {
		ctx.add_recv_task( [&] { listener->onNotify(p); } );
	}
}

void Client::close() {
	if(connected) {
		std::cout << "Shutting down client" << std::endl;
#ifdef _WIN32
		closesocket(m_sock_fd);
#else
		::close(m_sock_fd);
#endif
	} else {
		std::cout << "Client already disconnected" << std::endl;
	}
	connected = false;
}

} // namespace osf
