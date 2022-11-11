#pragma once

#include "Core/CoreInclude.hpp"
#include <AL\al.h>

namespace Pit::Audio {
	class SoundBuffer {
	public:
		static void Shutdown();

		static ALuint AddSoundEffect(const String& filename);
		static bool RemoveSoundEffect(const ALuint& buffer);

	private:
		static Array<ALuint> s_SoundEffectBuffers;
	};
}