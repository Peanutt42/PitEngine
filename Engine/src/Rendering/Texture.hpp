#pragma once

#include "Core/CoreInclude.hpp"
#include <vulkan/vulkan.h>

namespace Pit::Rendering {
	enum TextureFormat {
		None = 0,
		RGBA,
		RGBA32F
	};

	class Texture {
	public:
		Texture(const String& path);
		~Texture();

		void SetData(void* data);

		VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

		void Resize(uint32 width, uint32 height);

		uint32 GetWidth() const { return m_Width; }
		uint32 GetHeight() const { return m_Height; }

		const String& GetFilepath() {
			return m_Filepath;
		}

	private:
		void AllocateMemory(uint64 size);
		void Release();
	private:
		uint32 m_Width = 0, m_Height = 0;

		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VkDeviceMemory m_Memory = nullptr;
		VkSampler m_Sampler = nullptr;

		TextureFormat m_Format = TextureFormat::None;

		VkBuffer m_StagingBuffer = nullptr;
		VkDeviceMemory m_StagingBufferMemory = nullptr;

		size m_AlignedSize = 0;

		VkDescriptorSet m_DescriptorSet = nullptr;

		String m_Filepath;
		bool m_Png = true;
	};
}