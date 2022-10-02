#pragma once

#include "Main/CoreInclude.hpp"
#include "RenderingInclude.hpp"
#include "Rendering/Vulkan/VulkanDevice.hpp"
#include "Rendering/Vulkan/VulkanSwapChain.hpp"
#include "ImGui/ImGuiRenderer.hpp"
#include "Window.hpp"
#include "ECS/ECSWorld.hpp"

namespace Pit::Rendering {
	using namespace Vulkan;

	struct RendererSpecs {
		bool VSync = false;
	};

	class Renderer {
	public:
		Renderer(Vulkan::Device& device, Window& window, ECS::World* ecsWorld);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->getRenderPass();	}
		bool IsFrameInProgress() const { return m_IsFrameStarted; }
		VkCommandBuffer GetCurrentCommandBuffer() const {
			assert(m_IsFrameStarted && "Cannot get command buffer when frame is not in progress!");
			return m_CommandBuffers[m_CurrentImageIndex];
		}

		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

		void SetSpecs(const RendererSpecs& specs);

		bool ShouldClose();

	private:
		Vulkan::Device& m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		uint32_t m_CurrentImageIndex;
		bool m_IsFrameStarted;

		void _CreateCommandBuffers();
		void _FreeCommandBuffers();
		void _RecreateSwapChain();

		void _InitGLFW();
		void _ShutdownGLFW();

		ImGuiRenderer* m_ImGuiRenderer;
		ECS::World* m_ECSWorld;
		Window& m_CurrentWindow;
	};
}