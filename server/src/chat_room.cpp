#include "../include/chat_room.h"

using namespace osf;

#if DEBUG
std::ofstream audioLog("audio_log.txt");
std::ofstream textLog("text_log.txt");
std::ofstream videoLog("video_log.txt");
#endif

void ChatRoom::broadcast(const packet p, const id_t& source_id) {
    std::unique_lock<std::mutex> lock(m_mtx);

	if(p.type == PCKTYPE::TEXT) {
		std::cout << "FROM: " << source_id << std::endl;
		std::cout << "[MSG] " << p << std::endl;
		#if DEBUG
		textLog << "FROM: " << source_id << std::endl;
		textLog << "[MSG] " << p << std::endl;
		#endif
	} 
	#if DEBUG 
	else if (p.type == PCKTYPE::AUDIO) {
		audioLog << "Received: " << strlen(p.data) << " bytes of AUDIO data from " 
			<< source_id << std::endl;
	} else if (p.type == PCKTYPE::VIDEO) {
		audioLog << "Received: " << strlen(p.data) << " bytes of VIDEO data from " 
			<< source_id << std::endl;
	}
	#endif
    
	if(p.type == PCKTYPE::TEXT) {
		for(Connection& _client : m_clients) {
			if(!(_client.getId() == source_id)) {
				_client.send_text(p);
			}
		}
	}
	if(p.type == PCKTYPE::AUDIO) {
		for(Connection& _client : m_clients) {
			if(!(_client.getId() == source_id)) {
				_client.send_audio(p);
			}
		}
	}
    
}


void ChatRoom::add_client(socket_t _fd, sockaddr_in _addr) {
    std::unique_lock<std::mutex> lock(m_mtx);

    id_t new_client_id;

    do {
        new_client_id = rand();
    } while(client_ids.count(new_client_id));
    
    m_clients.emplace_back(recv_tp, send_tp, *this, 
        std::move(_fd), _addr, new_client_id);

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