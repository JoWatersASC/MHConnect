#include <RtAudio.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <cstdint>

namespace osf
{

static constexpr int SAMPLE_RATE = 44100;
static constexpr unsigned int BUFFER_FRAMES = 256;
static constexpr int CHANNELS = 2;
static constexpr int BYTES_PER_SAMPLE = sizeof(float);

class AudioStreamer {
public:
    AudioStreamer(int _fd) : m_sock_fd(_fd) {
        // Verify audio devices
        if (m_audio.getDeviceCount() < 1) {
            throw std::runtime_error("No audio devices found!");
        }
    }

    void startCapture();
    void stopCapture();

private:
    RtAudio m_audio;
    int& m_sock_fd;
    std::vector<char> m_audioBuffer;    

    static int recordCallback(void*, void*, unsigned int, double, RtAudioStreamStatus, void*);
};

}