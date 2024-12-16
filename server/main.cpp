#include "../include/server.h"

int main(int argc, char* argv[]) {
#ifdef _WIN32
if (winsock_initialize())
	return 1;
#endif

	if(argc == 2) {
		osf::PORT = std::stoi(argv[1]);
	}
	else {
		osf::PORT = 16000;
	}
	//std::cout << osf::PORT << std::endl;
	osf::Server s;
	s.start();

#ifdef _WIN32
	WSACleanup();
#endif
}
