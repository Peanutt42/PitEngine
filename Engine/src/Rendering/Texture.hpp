#pragma once

// Help from TheCherno/Hazel https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Platform/OpenGL/OpenGLTexture.h

#include "Core/CoreInclude.hpp"
#include <glad/glad.h>
#include <stb_image.h>

namespace Pit::Rendering {
	/// <summary>
	/// Helper class to load and use textures for the game
	/// </summary>
	class Texture {
	public:
		Texture(uint32_t width, uint32_t height);
		Texture(const std::string& path, bool asyncLoading = false);
		void FinishAsyncLoadingMainThread();
		virtual ~Texture();

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetRendererID() const { return m_RendererID; }

		const std::string& GetPath() const { return m_Path; }
		void SetData(void* data, uint32_t size);

		void Bind(int slot = 0) const;
		bool IsLoaded() const { return m_IsLoaded; }

		bool operator==(const Texture& other) const {
			return m_RendererID == other.GetRendererID();
		}
	private:
		std::string m_Path;
		bool m_IsLoaded = false;
		stbi_uc* m_TextureFileData;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}