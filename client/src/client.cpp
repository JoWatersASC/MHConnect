#include "../include/client.h"

using namespace osf;

void Client::start_connect() {
    int res = ::connect(m_sock_fd, (sockaddr *)&m_addr, sizeof(m_addr));

    if(res == 0) {
        std::cout << "Connected to " << inet_ntoa(m_addr.sin_addr);
        std::cout << " on port " << ntohs(m_addr.sin_port) << std::endl;
        connected = true;

        m_inaudio.startCapture();
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
    ssize_t bytes = recv_pckt(m_sock_fd, out_pckt);

    if(bytes > 0 && connected) {
        if(out_pckt.type == PCKTYPE::AUDIO) {
            m_outaudio.receiveAudioData(out_pckt);
            // std::cerr << "received audio" << std::endl;
        } else {
            std::cout << "Received: " << out_pckt << std::endl;
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
        packet out_pckt{};
        out_pckt.type = PCKTYPE::TEXT;
        out_pckt = msg;

        ssize_t bytes = send_pckt(m_sock_fd, out_pckt);

        if(bytes < 0) {
            std::cout << "Packet send failed: [ERROR] ";
            std::cout << strerror(errno) << std::endl;
        }
        
        std::cout << "[YOU] ";
    }

    close();
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