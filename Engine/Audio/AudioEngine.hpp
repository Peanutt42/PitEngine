#pragma once

#include "Main/CoreInclude.hpp"
#include <AL/alc.h>
#include <AL/al.h>
#include <queue>

namespace Pit::Audio {
	class MusicBuffer;

	struct MusicBufferEntry {
		uint id;
		MusicBuffer* MusicBuffer;
	};

	class AudioEngine {
	public:
		AudioEngine();
		~AudioEngine();

		uint AddMusicBuffer(MusicBuffer* musicBuffer);
		void RemoveMusicBuffer(uint id);

		void Update();

		void SetGain(float val) {
			float newVol = val;
			if (newVol < 0.f)
				newVol = 0.f;
			else if (newVol > 5.f)
				newVol = 5.f;

			alListenerf(AL_GAIN, newVol);
		}

	private:
		ALCdevice* p_ALCDevice;
		ALCcontext* p_ALCContext;

		std::vector<MusicBufferEntry> m_MusicBuffers;
		std::queue<uint> m_MusicBufferFreeIds;
	};
}