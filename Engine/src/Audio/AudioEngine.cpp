#include "Main/CoreInclude.hpp"
#include "AudioEngine.hpp"
#include "MusicBuffer.hpp"
#include <AL/al.h>

using namespace Pit::Audio;


AudioEngine::AudioEngine() {
	p_ALCDevice = alcOpenDevice(nullptr); // nullptr = get default device
	if (!p_ALCDevice)
		PIT_ENGINE_ERR(Log::Audio, "Failed to get sound device");

	p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);  // create context
	if (!p_ALCContext)
		PIT_ENGINE_ERR(Log::Audio, "Failed to set sound context");

	if (!alcMakeContextCurrent(p_ALCContext))  // make context current
		PIT_ENGINE_ERR(Log::Audio, "Failed to make context current");

	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR)
		name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);

	PIT_ENGINE_INFO(Log::Audio, "Opened '{}'", name);
}


AudioEngine::~AudioEngine() {
	m_MusicBuffers.clear();

	if (!alcMakeContextCurrent(nullptr))
		PIT_ENGINE_ERR(Log::Audio, "Failed to set context to nullptr");

	alcDestroyContext(p_ALCContext);

	if (!alcCloseDevice(p_ALCDevice))
		PIT_ENGINE_ERR(Log::Audio, "Failed to close sound device");
}

uint AudioEngine::AddMusicBuffer(MusicBuffer* musicBuffer) {
	static uint nextId = 0;

	if (!musicBuffer) return 0;

	uint id = 0;
	if (!m_MusicBufferFreeIds.empty()) {
		id = m_MusicBufferFreeIds.front();
		m_MusicBufferFreeIds.pop();
	}
	else
		id = nextId++;
	m_MusicBuffers.push_back({ id, musicBuffer });
	musicBuffer->Play();

	return nextId;
}

void AudioEngine::RemoveMusicBuffer(uint id) {
	for (uint i = 0; i < m_MusicBuffers.size(); i++) {
		if (m_MusicBuffers[i].id == id) {
			m_MusicBuffers.erase(m_MusicBuffers.begin() + i);
			m_MusicBufferFreeIds.push(id);
		}
	}
}

void AudioEngine::Update() {
	for (auto& musicBufferEntry : m_MusicBuffers) {
		musicBufferEntry.MusicBuffer->UpdateBufferStream();
	}
}