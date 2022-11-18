#pragma once

#include "Core/CoreInclude.hpp"
#include <AL\al.h>
#include <sndfile.h>

namespace Pit::Audio {

#define BUFFER_SAMPLES 8192
#define NUM_BUFFERS 4

	class MusicBuffer {
	public:
		MusicBuffer(const char* filename, bool asyncLoading = false);
		~MusicBuffer();

		void FinishAsyncLoadingOnMainThread();

		void Play();

		void UpdateBufferStream();

		ALint GetSource();

	private:
		ALuint m_Source;
		ALuint m_Buffers[NUM_BUFFERS];
		SNDFILE* m_SndFile;
		SF_INFO m_Sfinfo;
		short* m_Membuf;
		ALenum m_Format;

		MusicBuffer() = delete;
	};
}