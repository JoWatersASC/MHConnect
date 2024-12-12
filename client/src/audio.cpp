#include "../include/audio.h"

#include <algorithm>

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

    if (!streamer->isCapturing) {
        return 1; // Stop streaming
    }

    packet audio_pckt;
    audio_pckt.type = PCKTYPE::AUDIO;
    
    size_t dataSize = std::min((uint)PACK_LEN, num_bframes * CHANNELS * BYTES_PER_SAMPLE);
    std::memcpy(audio_pckt.data, in_buff, dataSize);

    // Send over network
    ssize_t bytes = send_pckt(streamer->m_sock_fd, audio_pckt);
    
    if (bytes < 0) {
        std::cerr << "Network send failed" << std::endl;
        streamer->isCapturing = false;
    }
    
    return 0;
}










void AudioReceiver::startAudioStream() {
    if (m_audio.isStreamOpen()) {
        m_audio.closeStream();
    }

    RtAudio::StreamParameters outputParams;
    outputParams.deviceId = m_audio.getDefaultOutputDevice();
    outputParams.nChannels = CHANNELS;

    m_audio.openStream(
        &outputParams, nullptr, RTAUDIO_FLOAT32, SAMPLE_RATE, (unsigned int *)&BUFFER_FRAMES,
        &playbackCallback, this
    );

    m_audio.startStream();
}

void AudioReceiver::stop() {
    m_isReceiving = false;

    if (m_audio.isStreamOpen()) {
        m_audio.stopStream();
        m_audio.closeStream();
    }
}

int AudioReceiver::playbackCallback(void* out_buff, void* in_buff, unsigned int num_bframes,
    double streamTime, RtAudioStreamStatus status, void* userData) 
{
    AudioReceiver* receiver = static_cast<AudioReceiver*>(userData);

    std::lock_guard<std::mutex> lock(receiver->m_bufferMutex);

    if (receiver->m_playbackBuffer.size() >= num_bframes * CHANNELS) {
        std::memcpy(out_buff, 
                    receiver->m_playbackBuffer.data(), 
                    num_bframes * CHANNELS * sizeof(float));
        receiver->m_playbackBuffer.erase(
            receiver->m_playbackBuffer.begin(), 
            receiver->m_playbackBuffer.begin() + num_bframes * CHANNELS
        );
        return 0;
    }

    memset(out_buff, 0, num_bframes * CHANNELS * sizeof(float));
    return 0;
}

void AudioReceiver::receiveAudioData(const packet _pckt) {
    if (_pckt.type == PCKTYPE::AUDIO) {
        const float* audioFloats = reinterpret_cast<const float*>(_pckt.data);
        size_t floatCount = PACK_LEN / sizeof(float);

        std::lock_guard<std::mutex> lock(m_bufferMutex);
        m_playbackBuffer.insert(
            m_playbackBuffer.end(), 
            audioFloats, 
            audioFloats + floatCount
        );
    }
}