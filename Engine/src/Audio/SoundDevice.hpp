#pragma once

#include "Core/CoreInclude.hpp"
#include <AL\alc.h>

namespace Pit::Audio {
	class SoundDevice
	{
	public:
		static void Init(), Shutdown();

	private:
		static ALCdevice* s_ALCDevice;
		static ALCcontext* s_ALCContext;

	};
}