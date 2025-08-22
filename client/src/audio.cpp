#include "audio.h"
#include "client.h"
#include "common.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <mutex>

using namespace osf;

static inline const char* apiName(RtAudio::Api a);
static void displayAudios();
//------------------------------
// RtAudio Duplex
//------------------------------
void AudioDuplex::start(client_context &cl_ctx) {
	RtAudio::StreamParameters outParams, inParams;

	outParams.deviceId = m_audio.getDefaultOutputDevice();
	outParams.nChannels = OUTCHANNELS;
	outParams.firstChannel = 0;

	inParams.deviceId = m_audio.getDefaultInputDevice();
	inParams.nChannels = INCHANNELS;
	inParams.firstChannel = 0;

	unsigned buffer_frames = BUFFER_FRAMES;
	unsigned sample_rate   = SAMPLE_RATE;

	audio_pair = { &cl_ctx, this };

	try {
		m_audio.openStream(
			&outParams,
			&inParams,
			RTAUDIO_FLOAT32,
			sample_rate,
			&buffer_frames,
			&callback,
			&audio_pair
		);
		m_audio.startStream();
		std::cout << "Started audio" << std::endl;
	} catch(RtAudioErrorType &e) { }

	audio_pair.ctx->client->addListener(this);
}

static std::vector<float> record_vect;
int AudioDuplex::callback(void *out_buff, void *in_buff, unsigned int num_bframes,
	double streamTime, unsigned int status, void *audio_context)
{
	packet p;
	p.type = PCKTYPE::AUDIO;

	decltype(audio_pair) *audio_ctx = static_cast<decltype(audio_ctx)>(audio_context);
	auto &audio_queue = audio_ctx->audio->recv_queue;

	if (status)
		std::cerr << "Stream over/underflow detected!" << std::endl;

	if(in_buff) {
		const unsigned samples = num_bframes * INCHANNELS;
		const size_t   bytes   = samples * sizeof(float);
		const size_t   payload = std::min(bytes, sizeof p.data);

		memcpy(&p.data, in_buff, payload);
		p.type = PCKTYPE::AUDIO;

		const auto &sock = audio_ctx->ctx->client->get_sock();
		audio_ctx->ctx->add_send_task([=]{
				send_pckt(sock, p);
		});
	}

	const unsigned samples = num_bframes * OUTCHANNELS;
	const size_t   bytes   = samples * sizeof(float);
	if(audio_queue.empty() || !out_buff)
		return 0;

	memset(out_buff, 0, bytes);

	auto frame = audio_queue.pop_front();

	const size_t len = std::min(bytes, sizeof(frame.data));
	memcpy(out_buff, frame.data, len);

	return 0;
}

static inline const char* apiName(RtAudio::Api a) {
	switch (a) {
		case RtAudio::LINUX_ALSA: return "ALSA";
		case RtAudio::UNIX_JACK:  return "JACK";
		case RtAudio::LINUX_PULSE:return "PulseAudio";
		case RtAudio::RTAUDIO_DUMMY: return "DUMMY";
		default: return "Other";
	}
}

static void displayAudios() {
	// Show compiled APIs
	std::vector<RtAudio::Api> compiled;
	RtAudio::getCompiledApi(compiled);
	std::cout << "Compiled RtAudio APIs:";
	for (auto a : compiled) std::cout << ' ' << apiName(a);
	std::cout << "\n";

	// Try preferred backends in order
	std::vector<RtAudio::Api> prefs = {
		RtAudio::LINUX_PULSE, RtAudio::LINUX_ALSA, RtAudio::UNIX_JACK
	};

	for (auto api : prefs) {
		if (std::find(compiled.begin(), compiled.end(), api) == compiled.end())
			continue;

		try {
			RtAudio rt(api);
			std::cout << "Using API: " << apiName(rt.getCurrentApi()) << "\n";
			unsigned count = rt.getDeviceCount();
			std::cout << "Device count: " << count << "\n";

			for (unsigned i = 0; i < count; ++i) {
				try {
					RtAudio::DeviceInfo di = rt.getDeviceInfo(i);
					std::cout << "  [" << i << "] " << di.name
							  << " | in=" << di.inputChannels
							  << " out=" << di.outputChannels
							  << " duplex=" << (di.duplexChannels ? "yes" : "no")
							  << (di.isDefaultInput ? " [default in]" : "")
							  << (di.isDefaultOutput ? " [default out]" : "")
							  << "\n";
				} catch (std::exception const& e) {
					std::cout << "  [" << i << "] error: " << e.what() << "\n";
				}
			}
		} catch (std::exception const& e) {
			std::cout << "Failed to init " << apiName(api) << ": " << e.what() << "\n";
			continue;
		}
	}
}
/*
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
	unsigned int bufferFrames = BUFFER_FRAMES;

	try {
		m_audio.openStream(
			nullptr,		 // Output parameters (null)
			&inputParams,	// Input parameters
			RTAUDIO_FLOAT32, // Audio format
			SAMPLE_RATE,	 // Sample rate
			&bufferFrames,   // Buffer frames
			&recordCallback, // Callback function
			this			 // Error callback
		);

		// Start audio stream
		m_audio.startStream();
	}
	catch (RtAudioErrorType& e) {
		std::cerr << "RtAudio Error: " << e << std::endl;
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
	catch (RtAudioErrorType& e) {
		std::cerr << "RtAudio Error: " << e << std::endl;
		throw;
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

	size_t dataSize = std::min((unsigned int)PACK_LEN, num_bframes * INCHANNELS * BYTES_PER_SAMPLE);
	// std::cout << dataSize << std::endl;
	std::memcpy(audio_pckt.data, reinterpret_cast<const Byte *>(in_buff), dataSize);

	// Send over network
	ssize_t bytes;
	{
		std::lock_guard<std::mutex> send_lock(send_mtx);
		bytes = send_pckt(streamer->m_sock_fd, audio_pckt);
	}

	if (bytes < 0) {
		std::cerr << "[ERROR] " << strerror(errno) << std::endl;
		streamer->isCapturing = false;
	}

	return 0;
}
*/

;
;
;
;
;
;
;


/*
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

	unsigned int bufferFrames = BUFFER_FRAMES;
	m_isReceiving = true;
	m_audio.openStream(
		&outputParams, nullptr, RTAUDIO_FLOAT32, SAMPLE_RATE, &bufferFrames,
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
*/
