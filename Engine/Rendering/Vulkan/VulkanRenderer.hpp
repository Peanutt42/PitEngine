#pragma once

#include "Rendering/RenderingInclude.hpp"
#include "Rendering/Renderer.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanModel.hpp"
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
		Window m_Window{ 800, 600, "Vulkan Tutorial" };
		Device m_Device{ m_Window };
		std::unique_ptr<SwapChain> m_SwapChain;
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::unique_ptr<Model> m_Model;

		void _CreatePipelineLayout();
		void _CreatePipeline();
		void _CreateCommandBuffers();
		void _DrawFrame();
		void _LoadModels();
		static void _Sierpinski(std::vector<Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);
		
		void _InitGLFW();
		void _ShutdownGLFW();

		void _RecreateSwapChain();
		void _RecordCommandBuffer(int imageIndex);
		void _FreeCommandBuffers();
	};
}