#include "pch.hpp"
#include "AudioSubmodule.hpp"
#include "SoundDevice.hpp"
#include "SoundBuffer.hpp"
#include "SoundSource.hpp"

using namespace Pit;
using namespace Audio;

void AudioSubmodule::Init() {
	PIT_PROFILE_FUNCTION();


	// "C:/Users/Peter/Downloads/iamtheprotectorofthissystem.wav"
	SoundDevice::Init();

	uint32_t /*ALuint*/ sound1 = SoundBuffer::AddSoundEffect("C:/Users/Peter/Downloads/iamtheprotectorofthissystem.wav");

	SoundSource mySpeaker;

	mySpeaker.Play(sound1);

}

void AudioSubmodule::Shutdown() {
	PIT_PROFILE_FUNCTION();

}

void AudioSubmodule::Update() {
	PIT_PROFILE_FUNCTION();

}