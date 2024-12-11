#include "include/client.h"

using namespace osf;

int main() {
    int cl_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr = create_address("127.0.0.1", 8080);

    Client firstc(cl_fd, serv_addr);
    firstc.start_connect();

    firstc.start_recv();
    firstc.start_send();

    firstc.close();
}