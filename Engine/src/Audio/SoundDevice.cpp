#include "pch.hpp"
#include "SoundDevice.hpp"
#include <AL\al.h>
#include <stdio.h>

using namespace Pit;
using namespace Audio;

SoundDevice::SoundDevice() {
	m_ALCDevice = alcOpenDevice(nullptr); // nullptr = get default device
	if (!m_ALCDevice)
		throw("failed to get sound device");

	m_ALCContext = alcCreateContext(m_ALCDevice, nullptr);  // create context
	if(!m_ALCContext)
		throw("Failed to set sound context");

	if (!alcMakeContextCurrent(m_ALCContext))   // make context current
		throw("failed to make context current");

	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(m_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(m_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(m_ALCDevice) != AL_NO_ERROR)
		name = alcGetString(m_ALCDevice, ALC_DEVICE_SPECIFIER);
	printf("Opened \"%s\"\n", name);
}

SoundDevice::~SoundDevice() {
	if (!alcMakeContextCurrent(nullptr))
		throw("failed to set context to nullptr");

	alcDestroyContext(m_ALCContext);
	if (m_ALCContext)
		throw("failed to unset during close");

	if (!alcCloseDevice(m_ALCDevice))
		throw("failed to close sound device");
}
