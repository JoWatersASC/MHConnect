#include "../include/server.h"

namespace osf
{

constexpr uint16_t PORT = 8080;

void Server::accept_client() {
    sockaddr_in c_addr;
    socklen_t c_addr_len = sizeof(sockaddr_in);
    int client_fd = accept(m_sock_fd, (sockaddr *)&c_addr, &c_addr_len);
    
    m_room.add_client(std::move(client_fd), std::move(c_addr));
}

void Server::start() {
    //create socket
    m_sock_fd = ::socket(AF_INET, SOCK_STREAM, 0);

    //create sockaddr
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //bind to port
    if(!::bind(m_sock_fd, (sockaddr *)&server_addr, sizeof(server_addr)))
            std::cout << "Server bound to port " << PORT << std::endl;
    else
        std::cerr << strerror(errno) << std::endl;

    //start listening on port
    if(!::listen(m_sock_fd, 10))
        std::cout << "Server started on port " << PORT << std::endl << std::endl;
    else
        std::cerr << strerror(errno) << std::endl;
    
    recv_tp.start();
    send_tp.start();

    running = true;
    do {
        //accept new client
        accept_client();
        
    } while(running);
}

}