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

		static bool init = false;
		if (!init) {
			AssetManagment::Asset* sample_asset = nullptr;
			for (auto asset : Engine::AssetManagment()->GetAssets()) {
				// TODO: Only for testing, next is some stable UUID that doesn't change to find assets
				if (asset->GetType() == AUDIO_ASSET_TYPE && asset->GetPath().ends_with("sample1.wav")) sample_asset = asset;
			}
			if (sample_asset && sample_asset->GetType() == AUDIO_ASSET_TYPE)
				music = Cast<Audio::AudioAsset*>(sample_asset)->Get();
			music->Play();
			init = true;
		}
		else
			music->UpdateBufferStream();
	}
}