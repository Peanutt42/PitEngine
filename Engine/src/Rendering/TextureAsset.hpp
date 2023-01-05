#pragma once

#include "Core/CoreInclude.hpp"
#include "Texture.hpp"
#include "AssetManagment/Asset.hpp"

namespace Pit::Rendering {
#define TEXTURE_ASSET_TYPE 2

	/// <summary>
	/// Asset type to load any texture you want
	/// </summary>
	class TextureAsset : public AssetManagment::Asset {
	public:
		TextureAsset(const std::filesystem::path& path, bool asyncLoading = false);

		~TextureAsset();

		virtual void Load() override;
		virtual void Unload() override;

		Texture* Get();

	private:
		bool m_AsyncLoading;
		ScopeRef<Texture> m_Texture;
	};
}