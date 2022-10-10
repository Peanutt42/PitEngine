#pragma once

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>

namespace Pit::Rendering {
	struct QueueFamilyIndices {
		uint32_t graphicsFamilyIndex;
		uint32_t computeFamilyIndex;
		uint32_t presentFamilyIndex;
	};

	struct SwapchainConfiguration {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct Frame {
		VkCommandPool       CommandPool;
		VkCommandBuffer     CommandBuffer;
		VkFence             Fence;
		VkImage             Backbuffer;
		VkImageView         BackbufferView;
		VkFramebuffer       Framebuffer;
	};

	struct FrameSemaphores {
		VkSemaphore         ImageAcquiredSemaphore;
		VkSemaphore         RenderCompleteSemaphore;
	};









	// === TO STRING === //
	static std::string PhysicalDeviceTypeToString(VkPhysicalDeviceType type) {
		switch (type) {
		default: return "UNKOWN";
		case VK_PHYSICAL_DEVICE_TYPE_CPU: return "CPU only";
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "External GPU";
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated GPU";
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "Virtual GPU";
		case VK_PHYSICAL_DEVICE_TYPE_OTHER: return "'Other' type";
		}
	}

	static std::string PresentModeToString(VkPresentModeKHR mode) {
		switch (mode) {
		default: return "UNKOWN";
		case VK_PRESENT_MODE_FIFO_KHR: return "Fifo";
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR: return "Relaxed Fifo";
		case VK_PRESENT_MODE_IMMEDIATE_KHR: return "Immediate";
		case VK_PRESENT_MODE_MAILBOX_KHR: return "Mailbox";
		case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR: return "Shared continuous refresh";
		case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR: return "Shared demand refresh";
		}
	}
}