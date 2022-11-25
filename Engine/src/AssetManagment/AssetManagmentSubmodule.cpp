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

		ScopedTimer t("AssetManagment load all assets from Sandbox/assets/.**");

		bool headless = Engine::GetSettings().Headless;

		for (auto& directoryEntry : std::filesystem::recursive_directory_iterator(std::filesystem::path(Pit::FileSystem::GetSandboxDir() + "assets"))) {
			auto path = directoryEntry.path();
			if (directoryEntry.is_directory()) continue;

			String fileExtention = path.extension().string();

			if (headless) continue;

			// Audio
			if (fileExtention == ".wav" ||
				fileExtention == ".ogg" ||
				fileExtention == ".mp3")
				m_Assets.push_back(new Audio::AudioAsset(path.string(), LOAD_AUDIO_ASYNC));

			// Textures
			else if (fileExtention == ".png" ||
					 fileExtention == ".jpg")
				std::cout << "Loading texture file: " << path.string() << '\n';
		}

		for (auto asset : m_Assets) {

			if (headless) continue;

			switch (asset->GetType()) {
			case NULL_ASSET_TYPE: PIT_ENGINE_ERR(AssetManagment, "Asset has invalid type."); break;
			case TEXTURE_ASSET_TYPE: Cast<Rendering::TextureAsset*>(asset)->Load(); break;
			case AUDIO_ASSET_TYPE: Cast<Audio::AudioAsset*>(asset)->Load(); break;
			};
		}
		JobSystem::Wait();

		// Finish async loading with synchronization on the main thread
		for ([[maybe_unused]] auto asset : m_Assets) {
			if (headless) continue;

#if LOAD_AUDIO_ASYNC
			if (asset->GetType() == AUDIO_ASSET_TYPE)
				Cast<Audio::AudioAsset*>(asset)->Get()->FinishAsyncLoadingOnMainThread();
#endif

#if LOAD_TEXTURE_ASYNC 
			if (asset->GetType() == TEXTURE_ASSET_TYPE)
				Cast<Rendering::TextureAsset*>(asset)->Get()->FinishAsyncLoadingMainThread();
#endif
		}
	}

	void AssetManagmentSubmodule::Shutdown() {
		PIT_PROFILE_FUNCTION();

		for (int i = 0; i < m_Assets.size(); i++)
			delete m_Assets[i];
	}

	void AssetManagmentSubmodule::Update() {
		PIT_PROFILE_FUNCTION();

	}
}