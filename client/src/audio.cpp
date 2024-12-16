#include "audio.h"

#include <algorithm>

using namespace osf;

void AudioStreamer::startCapture() {
    RtAudio::DeviceInfo info;
    unsigned int inputDevice = m_audio.getDefaultInputDevice();

    if (inputDevice == m_audio.getDeviceCount()) {
        std::cerr << "Invalid default input device" << std::endl;
        return;  // or handle error
    }
    
    // Audio stream parameters
    RtAudio::StreamParameters inputParams;
    inputParams.deviceId = inputDevice;
    inputParams.nChannels = INCHANNELS;
    inputParams.firstChannel = 0;

    isCapturing = true;

    try {
        m_audio.openStream(
            nullptr,               // Output parameters (null)
            &inputParams,          // Input parameters
            RTAUDIO_FLOAT32,       // Audio format
            SAMPLE_RATE,           // Sample rate
            (unsigned int *)&BUFFER_FRAMES,        // Buffer frames
            &recordCallback,       // Callback function
            this               // Error callback
        );

        // Start audio stream
        m_audio.startStream();
    }
    #ifdef _WIN32
    catch (RtAudioErrorType& e) {
        std::cerr << "RtAudio Error: " << e << std::endl;
        throw;
    }
    #else
    catch (RtAudioError& e) {
        std::cerr << "RtAudio Error: " << e.getMessage() << std::endl;
        throw;
    }
    #endif
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
    #ifdef _WIN32
    catch (RtAudioErrorType& e) {
        std::cerr << "RtAudio Error: " << e << std::endl;
        throw;
    }
    #else
    catch (RtAudioError& e) {
        std::cerr << "RtAudio Error: " << e.getMessage() << std::endl;
        throw;
    }
    #endif
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
    
    size_t dataSize = std::min((unsigned int)PACK_LEN, num_bframes * INCHANNELS * BYTES_PER_SAMPLE);
    // std::cout << dataSize << std::endl;
    std::memcpy(audio_pckt.data, reinterpret_cast<const Byte *>(in_buff), dataSize);

    // Send over network
    ssize_t bytes;
    {
        lockg send_lock(send_mtx);
        bytes = send_pckt(streamer->m_sock_fd, audio_pckt);
    }
    
    
    if (bytes < 0) {
        std::cerr << "[ERROR] " << strerror(errno) << std::endl;
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
    outputParams.nChannels = OUTCHANNELS;

    unsigned int outputDevice = m_audio.getDefaultOutputDevice();
    if (outputDevice == m_audio.getDeviceCount()) {
        std::cerr << "Invalid default input device" << std::endl;
        return;  // or handle error
    }

    m_isReceiving = true;
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

std::ofstream logger("logger.txt");
int AudioReceiver::playbackCallback(void* out_buff, void* in_buff, unsigned int num_bframes,
    double streamTime, RtAudioStreamStatus status, void* userData) 
{
    AudioReceiver* receiver = static_cast<AudioReceiver*>(userData);

    std::lock_guard<std::mutex> lock(receiver->m_bufferMutex);
    // logger << "received audio " << _pckt << std::endl;

    if (receiver->m_playbackBuffer.size() >= num_bframes * OUTCHANNELS) {
        std::memcpy(out_buff, 
                    receiver->m_playbackBuffer.data(), 
                    num_bframes * OUTCHANNELS * sizeof(float));
        receiver->m_playbackBuffer.erase(
            receiver->m_playbackBuffer.begin(), 
            receiver->m_playbackBuffer.begin() + num_bframes * OUTCHANNELS
        );
        return 0;
    }

    memset(out_buff, 0, num_bframes * OUTCHANNELS * sizeof(float));
    return 0;
}

void AudioReceiver::receiveAudioData(const packet& _pckt) {
    if (_pckt.type == PCKTYPE::AUDIO) {
        size_t dataSize = std::min((int)PACK_LEN, 1024);
        const float* audioFloats = reinterpret_cast<const float*>(_pckt.data);
        size_t floatCount = dataSize / sizeof(float);

        std::lock_guard<std::mutex> lock(m_bufferMutex);
        m_playbackBuffer.insert(
            m_playbackBuffer.end(), 
            audioFloats, 
            audioFloats + floatCount
        );
    }
}