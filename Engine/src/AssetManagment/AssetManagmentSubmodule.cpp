#include "pch.hpp"
#include "AssetManagmentSubmodule.hpp"
#include "Rendering/TextureAsset.hpp"
#include "Threading/JobSystem.hpp"
#include <stb_image.h>

using namespace Pit;

#define LOAD_TEXTURE_ASYNC true

void AssetManagmentSubmodule::Init() {
	PIT_PROFILE_FUNCTION();

	ScopedTimer t("AssetManagment load test textures");

	for (auto& directoryEntry : std::filesystem::recursive_directory_iterator(std::filesystem::path(Pit::FileSystem::GetSandboxDir() + "assets"))) {
		auto path = directoryEntry.path();
		if (directoryEntry.is_directory()) continue;

		String fileExtention = path.extension().string();
		if (fileExtention == ".png" || fileExtention == ".jpg") {
			Rendering::TextureAsset* texture = new Rendering::TextureAsset(path.string(), LOAD_TEXTURE_ASYNC);
			m_Assets.push_back(texture);
		}
	}

	for (auto asset : m_Assets) {
#if LOAD_TEXTURE_ASYNC
		JobSystem::Execute([asset]() {
#endif
			Rendering::TextureAsset* textureAsset = Cast<Rendering::TextureAsset*>(asset);
			if (textureAsset) {
				textureAsset->Load();
			}
#if LOAD_TEXTURE_ASYNC
		});
#endif
	}
	JobSystem::Wait();

#if LOAD_TEXTURE_ASYNC
	for (auto asset : m_Assets) {
		Rendering::TextureAsset* textureAsset = Cast<Rendering::TextureAsset*>(asset);
		if (textureAsset) textureAsset->Get()->FinishAsyncLoadingMainThread();
	}
#endif
}

void AssetManagmentSubmodule::Shutdown() {
	PIT_PROFILE_FUNCTION();

	for (int i = 0; i < m_Assets.size(); i++)
		delete m_Assets[i];
}

void AssetManagmentSubmodule::Update() {
	PIT_PROFILE_FUNCTION();

}