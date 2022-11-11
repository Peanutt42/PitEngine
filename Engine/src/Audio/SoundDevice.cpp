#include "pch.hpp"
#include "SoundDevice.hpp"
#include <AL\al.h>
#include <stdio.h>

using namespace Pit;
using namespace Audio;

ALCdevice* SoundDevice::s_ALCDevice;
ALCcontext* SoundDevice::s_ALCContext;

void SoundDevice::Init() {
	s_ALCDevice = alcOpenDevice(nullptr); // nullptr = get default device
	if (!s_ALCDevice)
		throw("failed to get sound device");

	s_ALCContext = alcCreateContext(s_ALCDevice, nullptr);  // create context
	if(!s_ALCContext)
		throw("Failed to set sound context");

	if (!alcMakeContextCurrent(s_ALCContext))   // make context current
		throw("failed to make context current");

	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(s_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(s_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(s_ALCDevice) != AL_NO_ERROR)
		name = alcGetString(s_ALCDevice, ALC_DEVICE_SPECIFIER);
	printf("Opened \"%s\"\n", name);
}

void SoundDevice::Shutdown() {
	if (!alcMakeContextCurrent(nullptr))
		throw("failed to set context to nullptr");

	alcDestroyContext(s_ALCContext);
	if (s_ALCContext)
		throw("failed to unset during close");

	if (!alcCloseDevice(s_ALCDevice))
		throw("failed to close sound device");
}
