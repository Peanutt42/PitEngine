#pragma once

#include "Main/CoreInclude.hpp"
#include "Main/Submodule.hpp"
#include "AudioEngine.hpp"
#include "MusicBuffer.hpp"

namespace Pit {
	DECLARE_EXTERN_PROFILE_STAT_FLOAT(AudioUpdate, Audio, "Audio-Update");

	class AudioSubmodule : public Submodule {
	public:
		virtual void Init() {
			AudioEngine = new Audio::AudioEngine();
			AudioEngine->SetGain(0.2f);
			sound1 = new Audio::MusicBuffer("C:/dev/cpp/PitEngine/Sandbox/assets/audio/mc-virgins-yun-head-trap-anthem-official-lyric-video.mp3");
			//sound1Id = AudioEngine->AddMusicBuffer(sound1);
		}

		virtual void Shutdown() {
			delete sound1;
			delete AudioEngine;
		}

		virtual void Update() {
			SCOPE_STAT(AudioUpdate);

			AudioEngine->Update();
		}

	private:
		Audio::AudioEngine* AudioEngine = nullptr;
		uint sound1Id = 0; Audio::MusicBuffer* sound1 = nullptr;
	};
}