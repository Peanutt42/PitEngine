#include "AudioEngine.hpp"
#include <AL/al.h>

using namespace Pit::Audio;

AudioEngine* AudioEngine::get() {
	static AudioEngine* snd_device = new AudioEngine();
	return snd_device;
}

AudioEngine::AudioEngine() {
	p_ALCDevice = alcOpenDevice(nullptr); // nullptr = get default device
	if (!p_ALCDevice) {
		PIT_ENGINE_ERR("Failed to get sound device");
	}

	p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);  // create context
	if (!p_ALCContext) {
		PIT_ENGINE_ERR("Failed to set sound context");
	}

	if (!alcMakeContextCurrent(p_ALCContext)) {  // make context current
		PIT_ENGINE_ERR("Failed to make context current");
	}

	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR)
		name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);
	PIT_ENGINE_INFO("Opened '{}'", name);
}

#pragma warning(disable: 4297) // Weird warning for exeptions not going to be triggered...

AudioEngine::~AudioEngine() {
	if (!alcMakeContextCurrent(nullptr)) {
		PIT_ENGINE_ERR("Failed to set context to nullptr");
	}

	alcDestroyContext(p_ALCContext);
	if (p_ALCContext) {
		PIT_ENGINE_ERR("Failed to unset during close");
	}

	if (!alcCloseDevice(p_ALCDevice)) {
		PIT_ENGINE_ERR("Failed to close sound device");
	}
}