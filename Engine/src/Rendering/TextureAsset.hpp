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
		TextureAsset(const String& path, bool asyncLoading = false);

		~TextureAsset();

		virtual void Load() override;
		virtual void Unload() override;

		Texture* Get();

	private:
		bool m_AsyncLoading;
		Texture* m_Texture = nullptr;
	};
}