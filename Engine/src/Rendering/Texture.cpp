#include "pch.hpp"
#include "Texture.hpp"
#include <stb_image.h>

namespace Pit::Rendering {

	Texture::Texture(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height) {

		PIT_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	Texture::Texture(const std::string& path, bool asyncLoading)
		: m_Path(path) {

		PIT_PROFILE_FUNCTION();

		int width, height, channels;

		stbi_set_flip_vertically_on_load(path.ends_with(".jpg"));
		m_TextureFileData = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (!m_TextureFileData) PIT_ENGINE_FATAL(Rendering, "Failed to properly load texture '{}'", path);


		m_IsLoaded = true;

		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		if (!(internalFormat & dataFormat))
			PIT_ENGINE_ERR(Rendering, "Format not supported!");

		if (!asyncLoading)
			FinishAsyncLoadingMainThread();
	}

	void Texture::FinishAsyncLoadingMainThread() {
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, m_DataFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, m_TextureFileData);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(m_TextureFileData);
	}

	Texture::~Texture() {
		glDeleteTextures(1, &m_RendererID);
	}

	uint32_t Texture::GetWidth() const { return m_Width; }

	uint32_t Texture::GetHeight() const { return m_Height; }

	uint32_t Texture::GetRendererID() const { return m_RendererID; }

	const std::string& Texture::GetPath() const { return m_Path; }

	void Texture::SetData(void* data, uint32_t size) {
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		if (size != m_Width * m_Height * bpp)
			PIT_ENGINE_FATAL(Rendering, "Texture::SetData expects the size to be width * height * format");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void Texture::Bind(int slot) const {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
	
	bool Texture::IsLoaded() const { return m_IsLoaded; }
	
	bool Texture::operator==(const Texture& other) const {
		return m_RendererID == other.GetRendererID();
	}
}