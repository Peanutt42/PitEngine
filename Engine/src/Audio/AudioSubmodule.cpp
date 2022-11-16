#include "pch.hpp"
#include "AudioSubmodule.hpp"
#include "MusicBuffer.hpp"

namespace Pit {
	static Audio::MusicBuffer* music;
	void AudioSubmodule::Init() {
		PIT_PROFILE_FUNCTION();

		m_Device = new Audio::SoundDevice();
		music = new Audio::MusicBuffer("C:/Users/Peter/Downloads/407640__drotzruhn__countdown-30-seconds.wav");
		music->Play();
	}

	void AudioSubmodule::Shutdown() {
		PIT_PROFILE_FUNCTION();

		delete music;
		delete m_Device;
	}

	void AudioSubmodule::Update() {
		PIT_PROFILE_FUNCTION();

		music->UpdateBufferStream();
	}
}