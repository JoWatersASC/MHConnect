#pragma once

#include "common.h"
#include "threadpool.h"
#include "audio.h"
#include "core.h"

#include<memory>
#include<algorithm>
#include<atomic>

class Interface;

namespace osf
{

class Client {
public:
	Client();
	Client(int _fd, sockaddr_in& _addr, size_t _nthreads)
	: m_sock_fd(_fd), m_addr(_addr),
	recv_tp(_nthreads / 3), send_tp(_nthreads * 2 / 3),
	m_inaudio(m_sock_fd, send_tp), m_outaudio(m_sock_fd, recv_tp) {
		send_tp.start();
		recv_tp.start();
	}

	bool start_connect();
	void start_recv();
	void start_send();
	void start_send(const std::string& msg);

	//GIT CHANGE, defined below 2 functions
	void start_audio_in() { m_inaudio.startCapture(); }
	void start_audio_out() { m_outaudio.startAudioStream(); }
	
	const packet poll_msgs() { return msg_queue.pop_front(); }
	const bool pollable() { return !msg_queue.empty(); }

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
	
	tqueue<packet> msg_queue;

public:
	class listener {
	public:
		virtual ~listener() {}
		virtual void onNotify(const packet&) = 0;
	};

	void addListener(listener* _listener) { listeners.push_back(_listener); }

private:
	std::vector<listener*> listeners;

	void notify() {
		if (!pollable())
			return;

		const packet p = msg_queue.pop_front();
		for (auto& listener : listeners) {
			listener->onNotify(p);
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
