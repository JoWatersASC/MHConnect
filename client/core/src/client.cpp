#include "../include/client.h"
#include "gui.h"

using namespace osf;

void Client::start_connect() {
    int res = ::connect(m_sock_fd, (sockaddr *)&m_addr, sizeof(m_addr));

    if(res == 0) {
        std::cout << "Connected to " << inet_ntoa(m_addr.sin_addr);
        std::cout << " on port " << ntohs(m_addr.sin_port) << std::endl;
        connected = true;

        m_outaudio.startAudioStream(); 
    } else {
        std::cout << "Connection failed: [ERROR] ";
        std::cout << strerror(errno) << std::endl;
        close();
    }
}

void Client::start_recv() {
    if(!connected) {
        std::cout << "Reception impossible, socket not connected" << std::endl;
        return;
    }

    packet out_pckt;
    ssize_t bytes;

    {
        lockg recv_lock(recv_mtx);
        bytes = recv_pckt(m_sock_fd, out_pckt);
    }

    if(bytes > 0 && connected) {
        if(out_pckt.type == PCKTYPE::AUDIO) {
            m_outaudio.receiveAudioData(out_pckt);
        } else if (out_pckt.type == PCKTYPE::TEXT){
            std::cout << "Received: " << out_pckt << std::endl;
			msg_queue.enqueue(out_pckt);
			m_gui->add_text(msg_queue.pop_front());
        }
        recv_tp.add([this]() { start_recv(); });
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
    std::cout << "[YOU] ";

    while(std::getline(std::cin, msg) && connected) {
        start_send(msg);
        
        std::cout << "[YOU] ";
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

void Client::close() {
    if(connected) {
        std::cout << "Shutting down client" << std::endl;
#ifdef _WIN32
        closesocket(m_sock_fd);
#else
        ::close(m_sock_fd);
#endif
        m_inaudio.stopCapture();
        m_outaudio.stop();
    } else {
        std::cout << "Client already disconnected" << std::endl;
    }
    connected = false;
}