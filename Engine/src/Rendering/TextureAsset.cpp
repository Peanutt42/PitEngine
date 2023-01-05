#include "pch.hpp"
#include "TextureAsset.hpp"

namespace Pit::Rendering {
	TextureAsset::TextureAsset(const std::filesystem::path& path, bool asyncLoading) : m_AsyncLoading(asyncLoading) {
		m_Filepath = path;
		m_TypeId = TEXTURE_ASSET_TYPE;
	}

	TextureAsset::~TextureAsset() {
		m_Texture.Release();
	}
	
	void TextureAsset::Load() {
		Unload();
		m_Texture = new Texture(m_Filepath, m_AsyncLoading);
	}
	
	void TextureAsset::Unload() {
		if (m_Texture) m_Texture.Release();
	}

	Texture* TextureAsset::Get() { return m_Texture.Get(); }
}