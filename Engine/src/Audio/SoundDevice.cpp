#include "pch.hpp"
#include "SoundDevice.hpp"
#include <AL/al.h>
#include <stdio.h>

namespace Pit::Audio {

	SoundDevice::SoundDevice() {
		m_ALCDevice = alcOpenDevice(nullptr); // nullptr = get default device
		if (!m_ALCDevice)
			PIT_ENGINE_FATAL(Audio, "Failed to get sound device");

		m_ALCContext = alcCreateContext(m_ALCDevice, nullptr);
		if (!m_ALCContext)
			PIT_ENGINE_FATAL(Audio, "Failed to set sound context");

		if (!alcMakeContextCurrent(m_ALCContext))
			PIT_ENGINE_FATAL(Audio, "Failed to make context current");

		const ALCchar* name = nullptr;
		if (alcIsExtensionPresent(m_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
			name = alcGetString(m_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
		if (!name || alcGetError(m_ALCDevice) != AL_NO_ERROR)
			name = alcGetString(m_ALCDevice, ALC_DEVICE_SPECIFIER);

		PIT_ENGINE_INFO(Audio, "OpenAL Info:");
		PIT_ENGINE_INFO(Audio, " - Vendor: {}", alGetString(AL_VENDOR));
		PIT_ENGINE_INFO(Audio, " - Renderer: {}", alGetString(AL_RENDERER));
		PIT_ENGINE_INFO(Audio, " - Version: {}", alGetString(AL_VERSION));
		PIT_ENGINE_INFO(Audio, " - Speaker: {}", name);
	}

	SoundDevice::~SoundDevice() {
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(m_ALCContext);
		alcCloseDevice(m_ALCDevice);
	}
}