#include "pch.hpp"
#include "AudioSubmodule.hpp"
#include "MusicBuffer.hpp"

using namespace Pit;
using namespace Audio;

static MusicBuffer* music;
void AudioSubmodule::Init() {
	PIT_PROFILE_FUNCTION();

	m_Device = new SoundDevice();
	music = new MusicBuffer("C:/Users/Peter/Downloads/407640__drotzruhn__countdown-30-seconds.wav");
	music->Play();
}

void AudioSubmodule::Shutdown() {
	PIT_PROFILE_FUNCTION();

	delete m_Device;
	delete music;
}

void AudioSubmodule::Update() {
	PIT_PROFILE_FUNCTION();

	ALint state = AL_PLAYING;
	alGetSourcei(music->GetSource(), AL_SOURCE_STATE, &state);
	if (state == AL_PLAYING && alGetError() == AL_NO_ERROR)
		music->UpdateBufferStream();
}