#include <RtAudio.h>
#include "common.h"
#include "threadpool.h"

namespace osf
{

static constexpr int SAMPLE_RATE = 44100;
static constexpr unsigned int BUFFER_FRAMES = 256;
static constexpr int CHANNELS = 2;
static constexpr int BYTES_PER_SAMPLE = sizeof(float);

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
    int& m_sock_fd;
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
    void receiveAudioData(const packet);
    void stop();

private:
    RtAudio m_audio;
    int& m_sock_fd;
    std::vector<float> m_playbackBuffer;
    std::mutex m_bufferMutex;
    std::atomic<bool> m_isReceiving = false;
    // std::thread m_receiveThread;

    ThreadPool& recv_tp;

    static constexpr int SAMPLE_RATE = 44100;
    static constexpr int BUFFER_FRAMES = 256;
    static constexpr int CHANNELS = 2;

    static int playbackCallback(void*, void*, unsigned int, double, RtAudioStreamStatus, void*);
};

}