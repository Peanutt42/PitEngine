#pragma once

#include "Main/CoreInclude.hpp"
#include "RenderingInclude.hpp"
#include "Rendering/Vulkan/VulkanDevice.hpp"
#include "Rendering/Vulkan/VulkanSwapChain.hpp"
#include "Window.hpp"
#include "ECS/ECSWorld.hpp"

namespace Pit::Rendering {
	using namespace Vulkan;

	class Renderer {
	public:
		Renderer(Vulkan::Device& device, Window& window, ECS::World* ecsWorld);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->getRenderPass();	}
		float GetAspectRatio() const { return m_SwapChain->extentAspectRatio(); }
		bool IsFrameInProgress() const { return m_IsFrameStarted; }
		VkCommandBuffer GetCurrentCommandBuffer() const {
			assert(m_IsFrameStarted && "Can't get command buffer when frame isn't in progress!");
			return m_CommandBuffers[m_CurrentFrameIndex];
		}

		int GetFrameIndex() const {
			assert(m_IsFrameStarted && "Can't get frameIndex when frame isn't in progress!");
			return m_CurrentFrameIndex;
		}

		VkCommandBuffer BeginFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndFrame();

		bool ShouldClose();

		uint32_t GetSwapChainImageCount() { return static_cast<uint32_t>(m_SwapChain->imageCount()); }

		VkDescriptorPool GetDescriptorPoolPool() { return m_DescriptorPool; }

	private:
		Vulkan::Device& m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		uint32_t m_CurrentImageIndex;
		int m_CurrentFrameIndex;
		bool m_IsFrameStarted;
		VkDescriptorPool m_DescriptorPool;

		void _CreateCommandBuffers();
		void _FreeCommandBuffers();
		void _RecreateSwapChain();
		void _CreateDecriptorPool();

		void _InitGLFW();
		void _ShutdownGLFW();

		ECS::World* m_ECSWorld;
		Window& m_CurrentWindow;
	};
}