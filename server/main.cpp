#include "../include/server.h"

int main(int argc, char* argv[]) {
	if(argc == 2) {
		std::cout << argv[1] << std::endl;
		osf::PORT = std::stoi(argv[1]);
	}
	osf::Server s;
	s.start();
}
