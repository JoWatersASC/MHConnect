#include "../include/audio.h"

using namespace osf;

void AudioStreamer::startCapture() {
    RtAudio::DeviceInfo info;
    unsigned int inputDevice = m_audio.getDefaultInputDevice();
    
    // Audio stream parameters
    RtAudio::StreamParameters inputParams;
    inputParams.deviceId = inputDevice;
    inputParams.nChannels = CHANNELS;
    inputParams.firstChannel = 0;

    try {
        m_audio.openStream(
            nullptr,               // Output parameters (null)
            &inputParams,          // Input parameters
            RTAUDIO_FLOAT32,       // Audio format
            SAMPLE_RATE,           // Sample rate
            (unsigned int *)&BUFFER_FRAMES,        // Buffer frames
            &recordCallback,       // Callback function
            this,                  // User data
            nullptr,               // Stream options
            nullptr                // Error callback
        );

        // Start audio stream
        m_audio.startStream();
    }
    catch (RtAudioError& e) {
        std::cerr << "RtAudio Error: " << e.getMessage() << std::endl;
        throw;
    }
}

void AudioStreamer::stopCapture() {
    try {
        // Stop and close stream
        if (m_audio.isStreamRunning()) {
            m_audio.stopStream();
        }
        if (m_audio.isStreamOpen()) {
            m_audio.closeStream();
        }
        
        // Close socket
        close(m_sock_fd);
    }
    catch (RtAudioError& e) {
        std::cerr << "RtAudio Cleanup Error: " << e.getMessage() << std::endl;
    }
}

int AudioStreamer::recordCallback(void* out_buff, void* in_buff, unsigned int num_bframes,
    double stream_time, RtAudioStreamStatus status, void* userData) 
{
    AudioStreamer* streamer = static_cast<AudioStreamer*>(userData);
    
    if (status == RTAUDIO_INPUT_OVERFLOW) {
        std::cerr << "Audio buffer overflow" << std::endl;
    }

    // Prepare network packet
    std::vector<char> packet(sizeof(uint32_t) + num_bframes * CHANNELS * BYTES_PER_SAMPLE);
    
    // Packet length (network byte order)
    uint32_t packetLength = num_bframes * CHANNELS * BYTES_PER_SAMPLE;
    uint32_t networkLength = htonl(packetLength);
    
    // Copy packet length
    std::memcpy(packet.data(), &networkLength, sizeof(uint32_t));
    
    // Copy audio data
    std::memcpy(packet.data() + sizeof(uint32_t), 
                in_buff, 
                packetLength);

    // Send over network
    ssize_t bytes = send(streamer->m_sock_fd, packet.data(), packet.size(), 0);
    
    return 0;
}