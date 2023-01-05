#include "pch.hpp"
#include <Core/Engine.hpp>
#include "AssetManagmentSubmodule.hpp"
#include "Rendering/TextureAsset.hpp"
#include "Audio/AudioAsset.hpp"
#include "Threading/JobSystem.hpp"
#include <stb_image.h>

namespace Pit {
#define LOAD_TEXTURE_ASYNC true
#define LOAD_AUDIO_ASYNC true

	void AssetManagmentSubmodule::Init() {
		PIT_PROFILE_FUNCTION();

		ScopedTimer t("AssetManagment load all assets from Sandbox/Assets/.**");

		bool headless = Engine::GetSettings().Headless;
		std::filesystem::path assetPath(Pit::FileSystem::GetSandboxDir() + "Assets");

		// will probably be higher than needed, since we don't load every file, but it's a good buffer
		size_t potentialFileSize = 0;
		for (auto& entry : std::filesystem::recursive_directory_iterator(assetPath))
			if (!entry.is_directory()) potentialFileSize++;
		
		m_Assets.reserve(potentialFileSize);
		for (auto& directoryEntry : std::filesystem::recursive_directory_iterator(assetPath)) {
			auto path = directoryEntry.path();
			if (directoryEntry.is_directory()) continue;

			String fileExtention = path.extension().string();

			if (headless) continue;


			// Audio
			if (fileExtention == ".wav" ||
				fileExtention == ".ogg" ||
				fileExtention == ".mp3") {
				std::cout << "Loading " << path.string() << '\n';
				m_Assets.emplace_back(new Audio::AudioAsset(path.string(), LOAD_AUDIO_ASYNC));
			}

			// Textures
			else if (fileExtention == ".png" ||
					 fileExtention == ".jpg") {
				std::cout << "Loading " << path.string() << '\n';
				m_Assets.emplace_back(new Rendering::TextureAsset(path.string(), LOAD_TEXTURE_ASYNC));
			}

		}

		for (int i = 0; i < m_Assets.size(); i++) {

			if (headless) continue;

			m_Assets[i]->Load();
		}
		JobSystem::Wait();

		// Finish async loading with synchronization on the main thread
		for (int i = 0; i < m_Assets.size(); i++) {
			if (headless) continue;

#if LOAD_AUDIO_ASYNC
			if (m_Assets[i]->GetType() == AUDIO_ASSET_TYPE)
				Cast<Audio::AudioAsset*>(m_Assets[i].Get())->Get()->FinishAsyncLoadingOnMainThread();
#endif

#if LOAD_TEXTURE_ASYNC 
			if (m_Assets[i]->GetType() == TEXTURE_ASSET_TYPE)
				Cast<Rendering::TextureAsset*>(m_Assets[i].Get())->Get()->FinishAsyncLoadingMainThread();
#endif
		}
	}

	void AssetManagmentSubmodule::Shutdown() {
		PIT_PROFILE_FUNCTION();
		
		m_Assets.clear();
	}

	void AssetManagmentSubmodule::Update() {
		PIT_PROFILE_FUNCTION();

	}
	Array<ScopeRef<AssetManagment::Asset>>& AssetManagmentSubmodule::GetAssets() { return m_Assets; }
}