#include "client_h.h"
#include "context.h"

struct TextOutput : osf::Client::listener {
	void onNotify(const packet &p) {
		if(p.type != PCKTYPE::TEXT)
			return;

		std::cout << "\33[2K\r" "Received: " << p << std::endl;
		std::cout << "[YOU] ";
		std::flush(std::cout);
	}
};

int main(int argc, char* argv[]) {
#ifdef _WIN32
	if (winsock_initialize())
		return 1;
#endif

	osf::client_context cl_ctx;
	int cl_fd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serv_addr;

	if(argc == 3)
		serv_addr = osf::create_address(argv[1], std::stoi(argv[2]));
	else
		serv_addr = osf::create_address("127.0.0.1", 16000);

	TextOutput t;
	osf::Client firstc(cl_fd, serv_addr, cl_ctx);
	firstc.addListener(&t);
	firstc.try_connect();

	cl_ctx.run();
	firstc.start_send();
	firstc.close();

#ifdef _WIN32
	WSACleanup();
#endif
}
