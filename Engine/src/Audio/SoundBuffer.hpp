#pragma once

#include "Core/CoreInclude.hpp"
#include <AL\al.h>

namespace Pit::Audio {
	class SoundBuffer {
	public:
		~SoundBuffer();

		ALuint AddSoundEffect(const String& filename);
		bool RemoveSoundEffect(const ALuint& buffer);

	private:
		Array<ALuint> m_SoundEffectBuffers;
	};
}