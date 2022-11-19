#pragma once

#include "Core/CoreInclude.hpp"
#include <AL\al.h>

namespace Pit::Audio {
	/// <summary>
	/// TODO: Maybe change/remove, since audio is now a asset type
	/// </summary>
	class SoundBuffer {
	public:
		~SoundBuffer();

		ALuint AddSoundEffect(const String& filename);
		bool RemoveSoundEffect(const ALuint& buffer);

	private:
		Array<ALuint> m_SoundEffectBuffers;
	};
}