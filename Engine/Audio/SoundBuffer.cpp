#include "CoreInclude.hpp"
#include "SoundBuffer.hpp"
#include <sndfile.h>
#include <cstddef>
#include <inttypes.h>
#include <AL/alext.h>
#include <malloc.h>

using namespace Pit::Audio;

SoundBuffer* SoundBuffer::get() {
	static SoundBuffer* sndbuf = new SoundBuffer();
	return sndbuf;
}

ALuint SoundBuffer::addSoundEffect(const char* filename) {

	ALenum err, format;
	ALuint buffer;
	SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;

	/* Open the audio file and check that it's usable. */
	sndfile = sf_open(filename, SFM_READ, &sfinfo);
	if (!sndfile) {
		PIT_ENGINE_ERR(Log::Audio, "Could not open audio in {}", filename);
		return 0;
	}
	if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
		PIT_ENGINE_ERR(Log::Audio, "Bad sample count in {0:s} {1:d})\n", filename, static_cast<int>(sfinfo.frames));
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
		PIT_ENGINE_ERR(Log::Audio, "Unsupported channel count: {}", sfinfo.channels);
		sf_close(sndfile);
		return 0;
	}

	/* Decode the whole audio file to a buffer. */
	membuf = Cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if (num_frames < 1) {
		free(membuf);
		sf_close(sndfile);
		PIT_ENGINE_ERR(Log::Audio, "Failed to read samples in {0:s} {1:d}", filename, Cast<int>(num_frames));
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

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR) {
		PIT_ENGINE_ERR(Log::Audio, "OpenAL Error: {}", Cast<const char*>(alGetString(err)));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return 0;
	}

	p_SoundEffectBuffers.push_back(buffer);  // add to the list of known buffers

	return buffer;
}

bool SoundBuffer::removeSoundEffect(const ALuint& buffer) {
	auto it = p_SoundEffectBuffers.begin();
	while (it != p_SoundEffectBuffers.end()) {
		if (*it == buffer) {
			alDeleteBuffers(1, &*it);

			it = p_SoundEffectBuffers.erase(it);

			return true;
		}
		else {
			++it;
		}
	}
	return false;  // couldn't find to remove
}


SoundBuffer::SoundBuffer() {
	p_SoundEffectBuffers.clear();

}

SoundBuffer::~SoundBuffer() {
	alDeleteBuffers(Cast<ALsizei>(p_SoundEffectBuffers.size()), p_SoundEffectBuffers.data());

	p_SoundEffectBuffers.clear();
}