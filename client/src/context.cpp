#include "context.h"
#include "client.h"

void osf::client_context::run(uint8_t nthreads_in, uint8_t nthreads_out) {
	if(!client->is_open()) {
		std::cerr << "[ERR] Client not connected to host\n";
		return;
	}

	run_threads_in(nthreads_in);
	run_threads_out(nthreads_out);

	client->start_recv();
}
