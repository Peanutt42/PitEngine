#pragma once

#include "Core/CoreInclude.hpp"
#include "AssetManagment/Asset.hpp"
#include "MusicBuffer.hpp"
#include "Threading/JobSystem.hpp"

namespace Pit::Audio {
#define AUDIO_ASSET_TYPE 1

	class AudioAsset : public AssetManagment::Asset {
	public:
		AudioAsset(const String& path) {
			m_Filepath = path;
			m_TypeId = AUDIO_ASSET_TYPE;
		}

		~AudioAsset() {
			if (m_MusicBuffer) delete m_MusicBuffer;
		}

		virtual void Load() {
			if (m_MusicBuffer) delete m_MusicBuffer;
			m_MusicBuffer = new MusicBuffer(m_Filepath.string().c_str());
		}

		virtual void Unload() {
			if (m_MusicBuffer)
				delete m_MusicBuffer;
		}

		MusicBuffer* Get() { return m_MusicBuffer; }

	private:
		MusicBuffer* m_MusicBuffer = nullptr;
	};
}