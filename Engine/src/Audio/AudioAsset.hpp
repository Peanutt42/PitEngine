#pragma once

#include "Core/CoreInclude.hpp"
#include "AssetManagment/Asset.hpp"
#include "MusicBuffer.hpp"
#include "Threading/JobSystem.hpp"

namespace Pit::Audio {
#define AUDIO_ASSET_TYPE 1

	class AudioAsset : public AssetManagment::Asset {
	public:
		AudioAsset(const String& path, bool asyncLoading = false) : m_AsyncLoading(asyncLoading) {
			m_Filepath = path;
			m_TypeId = AUDIO_ASSET_TYPE;
		}

		~AudioAsset() {
			if (m_MusicBuffer) delete m_MusicBuffer;
		}

		virtual void Load() {
			if (m_AsyncLoading)
				JobSystem::Execute([this]() { _Load(); });
			else
				_Load();
		}
		void _Load() {
			if (m_MusicBuffer) delete m_MusicBuffer;
			m_MusicBuffer = new MusicBuffer(m_Filepath.string().c_str());

		}

		virtual void Unload() {
			if (m_AsyncLoading)
				JobSystem::Execute([this]() { if (m_MusicBuffer) delete m_MusicBuffer; });
			else if (m_MusicBuffer)
				delete m_MusicBuffer;
		}

		MusicBuffer* Get() { return m_MusicBuffer; }

	private:
		bool m_AsyncLoading;
		MusicBuffer* m_MusicBuffer;
	};
}