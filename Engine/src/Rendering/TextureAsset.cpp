#include "pch.hpp"
#include "TextureAsset.hpp"

namespace Pit::Rendering {
	TextureAsset::TextureAsset(const String& path, bool asyncLoading) : m_AsyncLoading(asyncLoading) {
		m_Filepath = path;
		m_TypeId = TEXTURE_ASSET_TYPE;
	}

	TextureAsset::~TextureAsset() {
		if (m_Texture) delete m_Texture;
	}
	
	void TextureAsset::Load() {
		if (m_Texture) delete m_Texture;
		m_Texture = new Texture(m_Filepath, m_AsyncLoading);
	}
	
	void TextureAsset::Unload() {
		if (m_Texture) delete m_Texture;
	}

	Texture* TextureAsset::Get() { return m_Texture; }
}