#include "include/client.h"

using namespace osf;

int main(int argc, char* argv[]) {
#ifdef _WIN32
if (winsock_initialize())
    return 1;
#endif

    int cl_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;

    if(argc == 3)
        serv_addr = create_address(argv[1], std::stoi(argv[2]));
    else 
<<<<<<< HEAD
        serv_addr = create_address("10.15.51.101", 8000);
=======
        serv_addr = create_address("127.0.0.1", 16000);
>>>>>>> 019f930 (Added INCHANNELS and OUTCHANNELS to audio files instaed of a single CHANNELS variable)


    Client firstc(cl_fd, serv_addr, 6);
    firstc.start_connect();

    firstc.start_recv();
    firstc.start_send();

    firstc.close();

#ifdef _WIN32
    WSACleanup();
#endif
}
