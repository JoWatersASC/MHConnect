#include "../include/server.h"

namespace osf
{

void Server::accept_client() {
    sockaddr_in c_addr;
    socklen_t c_addr_len = sizeof(sockaddr_in);
    socket_t client_fd = accept(m_sock_fd, (sockaddr *)&c_addr, &c_addr_len);

#ifdef _WIN32
    if (client_fd == INVALID_SOCKET) {
        int error = WSAGetLastError();
        LPSTR error_message = NULL;

        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&error_message, 0, NULL);

        // Log the error message
        printf("Error: %s\n", error_message);

        // Free the allocated memory
        LocalFree(error_message);
    }
#endif
    socket_t p;
    m_room.add_client(std::move(client_fd), std::move(c_addr));
}

void Server::start() {
    m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

#ifdef _WIN32
    if (bind(m_sock_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR)
        std::cerr << "Error: " << WSAGetLastError() << '\n';
    else
        std::cout << "Server bound to port " << PORT << std::endl;

    if(listen(m_sock_fd, 10) == SOCKET_ERROR)
        std::cerr << "Error: " << WSAGetLastError() << '\n';
    else
        std::cout << "Started listening on port " << PORT << std::endl;
#else
    if(!::bind(m_sock_fd, (sockaddr *)&server_addr, sizeof(server_addr)))
            std::cout << "Server bound to port " << PORT << std::endl;
    else
        std::cerr << strerror(errno) << std::endl;

    if (!::listen(m_sock_fd, 10))
        std::cout << "Server started on port " << PORT << std::endl << std::endl;
    else
        std::cerr << strerror(errno) << std::endl;
#endif

    
    
    recv_tp.start();
    send_tp.start();

    running = true;
    do {
        //accept new client
        accept_client();
        
    } while(running);
}

}
