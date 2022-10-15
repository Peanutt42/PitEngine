#pragma once

#include "Main/CoreInclude.hpp"
#include <AL/al.h>
#include <sndfile.h>

namespace Pit::Audio {
	class SoundBuffer {
	public:
		static SoundBuffer* get();

		ALuint addSoundEffect(const char* filename);
		bool removeSoundEffect(const ALuint& buffer);

	private:
		SoundBuffer();
		~SoundBuffer();

		std::vector<ALuint> p_SoundEffectBuffers;
	};
}