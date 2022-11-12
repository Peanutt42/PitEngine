#include "pch.hpp"
#include "SoundBuffer.hpp"
#include "sndfile.h"
#include <inttypes.h>
#include <AL\alext.h>

using namespace Pit;
using namespace Audio;


SoundBuffer::~SoundBuffer() {
	alDeleteBuffers(Cast<ALsizei>(m_SoundEffectBuffers.size()), m_SoundEffectBuffers.data());

	m_SoundEffectBuffers.clear();
}


ALuint SoundBuffer::AddSoundEffect(const String& filename) {
	ALenum err, format;
	ALuint buffer;
	SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;

	sndfile = sf_open(filename.c_str(), SFM_READ, &sfinfo);
	if (!sndfile) {
		PIT_ENGINE_FATAL(Audio, "Could not open audio in {0}: {1}", filename, sf_strerror(sndfile));
		return 0;
	}
	if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
		PIT_ENGINE_FATAL(Audio, "Bad sample count in {0:s} ({1:d})", filename, sfinfo.frames);
		sf_close(sndfile);
		return 0;
	}

	/* Get the sound format, and figure out the OpenAL format */
	format = AL_NONE;
	if (sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;
	else if (sfinfo.channels == 3) {
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT2D_16;
	}
	else if (sfinfo.channels == 4) {
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT3D_16;
	}
	if (!format) {
		PIT_ENGINE_FATAL(Audio, "Unsupported channel count: {}", sfinfo.channels);
		sf_close(sndfile);
		return 0;
	}

	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if (num_frames < 1) {
		free(membuf);
		sf_close(sndfile);
		PIT_ENGINE_FATAL(Audio, "Failed to read samples in {0:s} ({1:d})", filename, num_frames);
		return 0;
	}
	num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

	/* Buffer the audio data into a new buffer object, then free the data and
	 * close the file.
	 */
	buffer = 0;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndfile);

	err = alGetError();
	if (err != AL_NO_ERROR) {
		PIT_ENGINE_FATAL(Audio, "OpenAL Error: {}", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return 0;
	}

	m_SoundEffectBuffers.push_back(buffer);

	return buffer;
}

bool SoundBuffer::RemoveSoundEffect(const ALuint& buffer) {
	auto it = m_SoundEffectBuffers.begin();
	while (it != m_SoundEffectBuffers.end()) {
		if (*it == buffer) {
			alDeleteBuffers(1, &*it);
			it = m_SoundEffectBuffers.erase(it);
			return true;
		}
		else
			++it;
	}
	return false;
}