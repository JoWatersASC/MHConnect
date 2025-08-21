#pragma once

#include <queue>
#ifdef _WIN32
#define _WINSOCKAPI_
#endif

#include <RtAudio.h>

#ifdef _WIN32
#undef _WINSOCKAPI_
#endif

#include "client.h"
#include "context.h"
#include "common.h"
#include "threadpool.h"

namespace osf
{

static const int SAMPLE_RATE = 44100;
static const unsigned int BUFFER_FRAMES = 64;
static const int INCHANNELS = 1;
static const int OUTCHANNELS = 2;
static const int BYTES_PER_SAMPLE = sizeof(float);

class AudioStreamer {
public:
    AudioStreamer(int _fd, ThreadPool& s_tp) 
    : m_sock_fd(_fd), send_tp(s_tp) {
        if (m_audio.getDeviceCount() < 1) {
            throw std::runtime_error("No audio devices found!");
        }
    }
    ~AudioStreamer() {
        if (m_audio.isStreamRunning()) {
            stopCapture();
        }
    }

    void startCapture();
    void stopCapture();

private:
    RtAudio m_audio;
    int m_sock_fd;
    std::vector<char> m_audioBuff;
    std::atomic<bool> isCapturing = false;

    ThreadPool& send_tp;

    static int recordCallback(void*, void*, unsigned int, double, RtAudioStreamStatus, void*);
};

class AudioReceiver {
public:
    AudioReceiver(int _fd, ThreadPool& r_tp)
    : m_sock_fd(_fd), recv_tp(r_tp) {
        if (m_audio.getDeviceCount() < 1) {
            throw std::runtime_error("No audio devices found!");
        }
    }
    ~AudioReceiver() {
        stop();
    }

    void startAudioStream();
    void receiveAudioData(const packet&);
    void stop();

private:
    RtAudio m_audio;
    int m_sock_fd;
    std::vector<float> m_playbackBuffer;
    std::mutex m_bufferMutex;
    std::atomic<bool> m_isReceiving = false;
    // std::thread m_receiveThread;

    ThreadPool& recv_tp;

    static int playbackCallback(void*, void*, unsigned int, double, RtAudioStreamStatus, void*);
};

typedef int RtAudioStreamStatus;
class AudioDuplex : public osf::Client::listener {
public:
	void start(client_context &cl_ctx);
	void stop() {
		m_out_audio.stopStream();
		m_in_audio.stopStream();
	}

	void onNotify(const packet &p) final {
		if(p.type != PCKTYPE::AUDIO)
			return;

		recv_queue.enqueue(p);
	}
    static int callback(void*, void*, unsigned int, double, unsigned int, void*);

private:
	RtAudio m_out_audio;
	RtAudio m_in_audio;

	struct { client_context *ctx; AudioDuplex *audio; } audio_pair;
	tqueue<packet> recv_queue;
	std::mutex m_mtx;
};

}
