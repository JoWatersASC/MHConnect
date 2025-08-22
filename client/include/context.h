#ifndef MHCCLIENT_CONTEXT_H
#define MHCCLIENT_CONTEXT_H

#include "threadpool.h"
#include <cstdint>
#include <mutex>
#include <thread>

class Interface;

namespace osf
{

class Client;
class AudioDuplex;

struct client_context {
	ThreadPool input_tp;
	ThreadPool output_tp;
	uint8_t num_in_threads;
	uint8_t num_out_threads;

	void run(uint8_t nthreads_in = 0, uint8_t nthreads_out = 0);

	void run_threads_in(uint8_t nthreads = 0) {
		if(nthreads < 0)
			nthreads = num_in_threads;
		if(nthreads == 0)
			nthreads = std::thread::hardware_concurrency() / 3;
		num_in_threads = nthreads;

		input_tp.reserve(nthreads);
		input_tp.start();
	}
	void run_threads_out(uint8_t nthreads = 0) {
		if(nthreads < 0)
			nthreads = num_out_threads;
		if(nthreads == 0)
			nthreads = std::thread::hardware_concurrency() * 2 / 3;
		num_out_threads = nthreads;

		output_tp.reserve(nthreads);
		output_tp.start();
	}

	template <typename F>
	void add_send_task(F&& _task) {
		output_tp.add([this, task = std::forward<F>(_task)]
		{
			std::lock_guard<std::mutex> lock(send_mtx);
			task();
		});
	}
	template <typename F>
	void add_recv_task(F&& _task) {
		output_tp.add([this, task = std::forward<F>(_task)]
		{
			task();
		});
	}

	std::mutex send_mtx; // won't need if using multiple ports for text, audio, video, etc.
	std::mutex recv_mtx; // won't need if using multiple ports for text, audio, video, etc.
	// sockaddr_in audio_sockaddr; - for udp implementation

	Client *client        = nullptr;
	AudioDuplex *audiod   = nullptr;
	Interface *interface_ = nullptr;
};

}

#endif
