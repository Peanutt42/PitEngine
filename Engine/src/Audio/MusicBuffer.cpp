#include "pch.hpp"
#include "MusicBuffer.hpp"
#include <cstddef>
#include <AL\alext.h>
#include <malloc.h>

using namespace Pit;
using namespace Audio;


void MusicBuffer::Play() {

	alGetError();

	alSourceRewind(m_Source);
	alSourcei(m_Source, AL_BUFFER, 0);

	ALsizei i;
	for (i = 0; i < NUM_BUFFERS; i++) {
		sf_count_t slen = sf_readf_short(m_SndFile, m_Membuf, BUFFER_SAMPLES);
		if (slen < 1) break;

		slen *= m_Sfinfo.channels * (sf_count_t)sizeof(short);
		alBufferData(m_Buffers[i], m_Format, m_Membuf, (ALsizei)slen, m_Sfinfo.samplerate);
	}
	if (alGetError() != AL_NO_ERROR)
		PIT_ENGINE_FATAL(Audio, "Error buffering for playback");

	alSourceQueueBuffers(m_Source, i, m_Buffers);
	alSourcePlay(m_Source);
	if (alGetError() != AL_NO_ERROR)
		PIT_ENGINE_FATAL(Audio, "Error starting playback");
}

void MusicBuffer::UpdateBufferStream() {
	alGetError();

	ALint processed, state;
	alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
	alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &processed);
	if (alGetError() != AL_NO_ERROR)
		PIT_ENGINE_FATAL(Audio, "Error checking music source state");

	/* Unqueue and handle each processed buffer */
	while (processed > 0) {
		ALuint bufid;
		sf_count_t slen;

		alSourceUnqueueBuffers(m_Source, 1, &bufid);
		processed--;

		slen = sf_readf_short(m_SndFile, m_Membuf, BUFFER_SAMPLES);
		if (slen > 0) {
			slen *= m_Sfinfo.channels * (sf_count_t)sizeof(short);
			alBufferData(bufid, m_Format, m_Membuf, (ALsizei)slen,
						 m_Sfinfo.samplerate);
			alSourceQueueBuffers(m_Source, 1, &bufid);
		}
		if (alGetError() != AL_NO_ERROR)
			PIT_ENGINE_FATAL(Audio, "Error buffering music data");
	}

	/* Make sure the source hasn't underrun */
	if (state != AL_PLAYING && state != AL_PAUSED) {
		ALint queued;

		/* If no buffers are queued, playback is finished */
		alGetSourcei(m_Source, AL_BUFFERS_QUEUED, &queued);
		if (queued == 0)
			return;

		alSourcePlay(m_Source);
		if (alGetError() != AL_NO_ERROR)
			PIT_ENGINE_FATAL(Audio, "Error restarting music playback");
	}

}

ALint MusicBuffer::GetSource() {
	return m_Source;
}

MusicBuffer::MusicBuffer(const char* filename) {
	alGenSources(1, &m_Source);
	alGenBuffers(NUM_BUFFERS, m_Buffers);

	std::size_t frame_size;

	m_SndFile = sf_open(filename, SFM_READ, &m_Sfinfo);
	if (!m_SndFile)
		PIT_ENGINE_FATAL(Audio, "Could not open provided music file -- check path");

	/* Get the sound format, and figure out the OpenAL format */
	if (m_Sfinfo.channels == 1)
		m_Format = AL_FORMAT_MONO16;
	else if (m_Sfinfo.channels == 2)
		m_Format = AL_FORMAT_STEREO16;
	else if (m_Sfinfo.channels == 3) {
		if (sf_command(m_SndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			m_Format = AL_FORMAT_BFORMAT2D_16;
	}
	else if (m_Sfinfo.channels == 4) {
		if (sf_command(m_SndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			m_Format = AL_FORMAT_BFORMAT3D_16;
	}
	if (!m_Format) {
		sf_close(m_SndFile);
		m_SndFile = NULL;
		PIT_ENGINE_FATAL(Audio, "Unsupported channel count from file");
	}

	frame_size = ((size_t)BUFFER_SAMPLES * (size_t)m_Sfinfo.channels) * sizeof(short);
	m_Membuf = static_cast<short*>(malloc(frame_size));

}

MusicBuffer::~MusicBuffer() {
	alDeleteSources(1, &m_Source);

	if (m_SndFile)
		sf_close(m_SndFile);

	m_SndFile = nullptr;

	free(m_Membuf);

	alDeleteBuffers(NUM_BUFFERS, m_Buffers);

}