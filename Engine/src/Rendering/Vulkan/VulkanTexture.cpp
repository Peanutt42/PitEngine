#include "Main/CoreInclude.hpp"
#include "VulkanTexture.hpp"
#include "Main/Engine.hpp"
#include <stb_image.h>

using namespace Pit::Rendering;

static void check_vk_result(VkResult err) {
	if (err == 0) return;

	PIT_ENGINE_ERR(Log::Rendering, "[Vulkan] VkResult: %d", err);

	if (err < 0) abort();
}

static uint32_t GetVulkanMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits) {
	VkPhysicalDeviceMemoryProperties prop;
	vkGetPhysicalDeviceMemoryProperties(Pit::Engine::Rendering()->GetRenderer()->Device.getPhysicalDevice(), &prop);
	for (uint32_t i = 0; i < prop.memoryTypeCount; i++) {
		if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
			return i;
	}

	return 0xffffffff;
}

static uint32_t BytesPerPixel(TextureFormat format) {
	switch (format) {
	case TextureFormat::RGBA:    return 4;
	case TextureFormat::RGBA32F: return 16;
	}
	return 0;
}

static VkFormat WalnutFormatToVulkanFormat(TextureFormat format) {
	switch (format) {
	case TextureFormat::RGBA:    return VK_FORMAT_R8G8B8A8_UNORM;
	case TextureFormat::RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
	}
	return (VkFormat)0;
}


Texture::Texture(const std::string& path) : m_Filepath(path) {
	int width, height, channels;
	uint8_t* data = nullptr;

	stbi_set_flip_vertically_on_load(m_Filepath.ends_with(".png") ? 1 : 0);
	
	if (stbi_is_hdr(m_Filepath.c_str())) {
		data = (uint8_t*)stbi_loadf(m_Filepath.c_str(), &width, &height, &channels, 4);
		m_Format = TextureFormat::RGBA32F;
	}
	else {
		data = stbi_load(m_Filepath.c_str(), &width, &height, &channels, 4);
		m_Format = TextureFormat::RGBA;
	}

	m_Width = width;
	m_Height = height;

	AllocateMemory(m_Width * m_Height * BytesPerPixel(m_Format));
	SetData(data);
}

Texture::~Texture() {
	Release();
}

void Texture::SetData(void* data) {
	auto* renderer = Pit::Engine::Rendering()->GetRenderer();
	VkDevice device = renderer->Device.device();

	size_t upload_size = m_Width * m_Height * BytesPerPixel(m_Format);

	VkResult err;

	if (!m_StagingBuffer) {
		// Create the Upload Buffer
		{
			VkBufferCreateInfo buffer_info = {};
			buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			buffer_info.size = upload_size;
			buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			err = vkCreateBuffer(device, &buffer_info, nullptr, &m_StagingBuffer);
			check_vk_result(err);
			VkMemoryRequirements req;
			vkGetBufferMemoryRequirements(device, m_StagingBuffer, &req);
			m_AlignedSize = req.size;
			VkMemoryAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = req.size;
			alloc_info.memoryTypeIndex = GetVulkanMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
			err = vkAllocateMemory(device, &alloc_info, nullptr, &m_StagingBufferMemory);
			check_vk_result(err);
			err = vkBindBufferMemory(device, m_StagingBuffer, m_StagingBufferMemory, 0);
			check_vk_result(err);
		}

	}

	// Upload to Buffer
	{
		char* map = NULL;
		err = vkMapMemory(device, m_StagingBufferMemory, 0, m_AlignedSize, 0, (void**)(&map));
		check_vk_result(err);
		memcpy(map, data, upload_size);
		VkMappedMemoryRange range[1] = {};
		range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range[0].memory = m_StagingBufferMemory;
		range[0].size = m_AlignedSize;
		err = vkFlushMappedMemoryRanges(device, 1, range);
		check_vk_result(err);
		vkUnmapMemory(device, m_StagingBufferMemory);
	}

	// Release image memory using stb  
	stbi_image_free(data);

	// Create a command buffer that will perform following steps when hit in the command queue.
	// TODO: this works in the example, but may need input if this is an acceptable way to access the pool/create the command buffer.
	VkCommandBuffer command_buffer;
	{
		VkCommandBufferAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = Pit::Engine::Rendering()->GetRenderer()->Device.getCommandPool();
		alloc_info.commandBufferCount = 1;

		err = vkAllocateCommandBuffers(renderer->Device.device(), &alloc_info, &command_buffer);
		check_vk_result(err);

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(command_buffer, &begin_info);
		check_vk_result(err);
	}

	// Copy to Image
	{
		VkImageMemoryBarrier copy_barrier = {};
		copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		copy_barrier.image = m_Image;
		copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy_barrier.subresourceRange.levelCount = 1;
		copy_barrier.subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &copy_barrier);

		VkBufferImageCopy region = {};
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.layerCount = 1;
		region.imageExtent.width = m_Width;
		region.imageExtent.height = m_Height;
		region.imageExtent.depth = 1;
		vkCmdCopyBufferToImage(command_buffer, m_StagingBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		VkImageMemoryBarrier use_barrier = {};
		use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		use_barrier.image = m_Image;
		use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		use_barrier.subresourceRange.levelCount = 1;
		use_barrier.subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &use_barrier);
	}

	// End command buffer
	{
		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &command_buffer;
		err = vkEndCommandBuffer(command_buffer);
		check_vk_result(err);
		err = vkQueueSubmit(renderer->Device.queue(), 1, &end_info, VK_NULL_HANDLE);
		check_vk_result(err);
		err = vkDeviceWaitIdle(renderer->Device.device());
		check_vk_result(err);
	}
}

void Texture::Resize(uint32_t width, uint32_t height) {
	if (m_Image && m_Width == width && m_Height == height)
		return;

	// TODO: max size?

	m_Width = width;
	m_Height = height;

	Release();
	AllocateMemory(m_Width * m_Height * BytesPerPixel(m_Format));
}

void Texture::AllocateMemory(uint64_t size) {
	VkDevice device = Pit::Engine::Rendering()->GetRenderer()->Device.device();

	VkResult err;

	VkFormat vulkanFormat = WalnutFormatToVulkanFormat(m_Format);

	// Create the Image
	{
		VkImageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.imageType = VK_IMAGE_TYPE_2D;
		info.format = vulkanFormat;
		info.extent.width = m_Width;
		info.extent.height = m_Height;
		info.extent.depth = 1;
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		err = vkCreateImage(device, &info, nullptr, &m_Image);
		check_vk_result(err);
		VkMemoryRequirements req;
		vkGetImageMemoryRequirements(device, m_Image, &req);
		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = req.size;
		alloc_info.memoryTypeIndex = GetVulkanMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
		err = vkAllocateMemory(device, &alloc_info, nullptr, &m_Memory);
		check_vk_result(err);
		err = vkBindImageMemory(device, m_Image, m_Memory, 0);
		check_vk_result(err);
	}

	// Create the Image View:
	{
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.image = m_Image;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = vulkanFormat;
		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.layerCount = 1;
		err = vkCreateImageView(device, &info, nullptr, &m_ImageView);
		check_vk_result(err);
	}

	// Create sampler:
	{
		VkSamplerCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		info.magFilter = VK_FILTER_LINEAR;
		info.minFilter = VK_FILTER_LINEAR;
		info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		info.minLod = -1000;
		info.maxLod = 1000;
		info.maxAnisotropy = 1.0f;
		VkResult err = vkCreateSampler(device, &info, nullptr, &m_Sampler);
		check_vk_result(err);
	}

	// Create the Descriptor Set:
	m_DescriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Texture::Release() {
	VkDevice device = Pit::Engine::Rendering()->GetRenderer()->Device.device();

	vkDestroySampler(device, m_Sampler, nullptr);
	vkDestroyImageView(device, m_ImageView, nullptr);
	vkDestroyImage(device, m_Image, nullptr);
	vkFreeMemory(device, m_Memory, nullptr);
	vkDestroyBuffer(device, m_StagingBuffer, nullptr);
	vkFreeMemory(device, m_StagingBufferMemory, nullptr);

	m_Sampler = nullptr;
	m_ImageView = nullptr;
	m_Image = nullptr;
	m_Memory = nullptr;
	m_StagingBuffer = nullptr;
	m_StagingBufferMemory = nullptr;
}