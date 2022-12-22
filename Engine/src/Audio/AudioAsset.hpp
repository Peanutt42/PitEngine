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
		AudioAsset(const String& path, bool asyncLoading = false);
		~AudioAsset();

		virtual void Load() override;
		virtual void Unload() override;

		MusicBuffer* Get();

	private:
		bool m_AsyncLoading;
		MusicBuffer* m_MusicBuffer = nullptr;
	};
}