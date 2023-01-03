#include "pch.hpp"
#include "AudioAsset.hpp"

namespace Pit::Audio {
	AudioAsset::AudioAsset(const std::filesystem::path& path, bool asyncLoading)
		: m_AsyncLoading(asyncLoading) {
		m_Filepath = path;
		m_TypeId = AUDIO_ASSET_TYPE;
	}

	AudioAsset::~AudioAsset() {
		if (m_MusicBuffer) delete m_MusicBuffer;
	}
	
	void AudioAsset::Load() {
		if (m_MusicBuffer) delete m_MusicBuffer;
		if (m_AsyncLoading)
			JobSystem::Execute([this]() {
			m_MusicBuffer = new MusicBuffer(m_Filepath.string().c_str(), m_AsyncLoading);
		});
		else
			m_MusicBuffer = new MusicBuffer(m_Filepath.string().c_str(), m_AsyncLoading);
	}
	
	void AudioAsset::Unload() {
		if (m_MusicBuffer)
			delete m_MusicBuffer;
	}

	MusicBuffer* AudioAsset::Get() { return m_MusicBuffer; }
}