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
		TextureAsset(const String& path, bool asyncLoading = false) : m_AsyncLoading(asyncLoading) {
			m_Filepath = std::filesystem::path(path);
			m_TypeId = TEXTURE_ASSET_TYPE;
		}

		~TextureAsset() {
			if (m_Texture) delete m_Texture;
		}

		virtual void Load() override {
			if (m_Texture) delete m_Texture;
			m_Texture = new Texture(m_Filepath.string(), m_AsyncLoading);
		}

		virtual void Unload() override {
			if (m_Texture) delete m_Texture;
		}

		Texture* Get() { return m_Texture; }

	private:
		bool m_AsyncLoading;
		Texture* m_Texture = nullptr;
	};
}