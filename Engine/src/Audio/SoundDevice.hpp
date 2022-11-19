#pragma once

#include "Core/CoreInclude.hpp"
#include <AL\alc.h>

namespace Pit::Audio {
	/// <summary>
	/// Simple Wrapper around initializing OpenAl-soft
	/// </summary>
	class SoundDevice
	{
	public:
		SoundDevice();
		~SoundDevice();

	private:
		ALCdevice* m_ALCDevice;
		ALCcontext* m_ALCContext;
	};
}