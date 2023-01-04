#include "pch.hpp"
#include "Core/Engine.hpp"
#include "AssetManagment/AssetManagmentSubmodule.hpp"
#include "Audio/AudioAsset.hpp"
#include "AudioSubmodule.hpp"
#include "MusicBuffer.hpp"

namespace Pit {
	static Audio::MusicBuffer* music;
	void AudioSubmodule::Init() {
		PIT_PROFILE_FUNCTION();

		m_Device = new Audio::SoundDevice();		
	}

	void AudioSubmodule::Shutdown() {
		PIT_PROFILE_FUNCTION();

		delete m_Device;
	}

	void AudioSubmodule::Update() {
		PIT_PROFILE_FUNCTION();
	}
}