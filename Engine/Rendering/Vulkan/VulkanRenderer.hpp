#pragma once

#include "Main/CoreInclude.hpp"
#include "Rendering/RenderingInclude.hpp"
#include "Rendering/Renderer.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanSwapChain.hpp"
#include <memory>

namespace Pit::Rendering::Vulkan {
	class Renderer : public IRenderer { 
	public:
		Renderer();
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		virtual void  Render() override;

		virtual void SetWindow(Window* window) override;
		virtual void SetSpecs(const RendererSpecs& specs) override;

		virtual bool ShouldClose() override;

	private:
		Window m_Window{ 800, 800, "Hello Vulkan!" };
		Device m_Device{ m_Window };
		SwapChain m_SwapChain{ m_Device, m_Window.GetExtent(), false };

		void _CreatePipelineLayout();
		void _CreatePipeline();
		void _CreateCommandBuffers();
		void _DrawFrame();
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		
		void _InitGLFW();
		void _ShutdownGLFW();

		// GLFW callbacks
		static void _SetFramebufferSizeCallback(GLFWwindow* window, int width, int height);
		static void _SetWindowSizeCallback(GLFWwindow* window, int width, int height);
		static void _SetWindowPosCallback(GLFWwindow* window, int xpos, int ypos);
	};
}