#pragma once

#include "Core/CoreInclude.hpp"
#include "AssetManagment/Asset.hpp"
#include "MusicBuffer.hpp"
#include "Threading/JobSystem.hpp"

namespace Pit::Audio {
#define AUDIO_ASSET_TYPE 1

	/// <summary>
	/// Asset type for audio like sound effects or music
	/// </summary>
	class AudioAsset : public AssetManagment::Asset {
	public:
		AudioAsset(const String& path, bool asyncLoading = false) 
			: m_AsyncLoading(asyncLoading) {
			m_Filepath = path;
			m_TypeId = AUDIO_ASSET_TYPE;
		}

		~AudioAsset() {
			if (m_MusicBuffer) delete m_MusicBuffer;
		}

		virtual void Load() {
			if (m_MusicBuffer) delete m_MusicBuffer;
			if (m_AsyncLoading)
				JobSystem::Execute([this]() {
					m_MusicBuffer = new MusicBuffer(m_Filepath.string().c_str(), m_AsyncLoading);
				});
			else
				m_MusicBuffer = new MusicBuffer(m_Filepath.string().c_str(), m_AsyncLoading);
		}

		virtual void Unload() {
			if (m_MusicBuffer)
				delete m_MusicBuffer;
		}

		MusicBuffer* Get() { return m_MusicBuffer; }

	private:
		bool m_AsyncLoading;
		MusicBuffer* m_MusicBuffer = nullptr;
	};
}