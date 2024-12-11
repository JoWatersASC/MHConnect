#include "include/client.h"

using namespace osf;

int main(int argc, char* argv[]) {
    int cl_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr = create_address(argv[1], std::stoi(argv[2]));

    Client firstc(cl_fd, serv_addr);
    firstc.start_connect();

    firstc.start_recv();
    firstc.start_send();

    firstc.close();
}
