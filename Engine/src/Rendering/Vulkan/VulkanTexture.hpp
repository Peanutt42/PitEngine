#pragma once

#include "Main/CoreInclude.hpp"
#include <vulkan/vulkan.h>

namespace Pit::Rendering {
	enum TextureFormat {
		None = 0,
		RGBA,
		RGBA32F
	};

	class Texture {
	public:
		Texture(const std::string& path);
		~Texture();

		void SetData(void* data);

		VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

		void Resize(uint32_t width, uint32_t height);

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		const std::string& GetFilepath() {
			return m_Filepath;
		}

	private:
		void AllocateMemory(uint64_t size);
		void Release();
	private:
		uint32_t m_Width = 0, m_Height = 0;

		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VkDeviceMemory m_Memory = nullptr;
		VkSampler m_Sampler = nullptr;

		TextureFormat m_Format = TextureFormat::None;

		VkBuffer m_StagingBuffer = nullptr;
		VkDeviceMemory m_StagingBufferMemory = nullptr;

		size_t m_AlignedSize = 0;

		VkDescriptorSet m_DescriptorSet = nullptr;

		std::string m_Filepath;
		bool m_Png = true;
	};
}