#pragma once

#include "Main/CoreInclude.hpp"
#include <AL/alc.h>

namespace Pit::Audio {
	class AudioEngine {
	public:
		static AudioEngine* get();

	private:
		AudioEngine();
		~AudioEngine();

		ALCdevice* p_ALCDevice;
		ALCcontext* p_ALCContext;

	};
}