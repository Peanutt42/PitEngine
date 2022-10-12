#pragma once

#include "Main/CoreInclude.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanMesh.hpp"
#include "Window.hpp"

namespace Pit::Rendering {
	struct RendererContext {
		Window* m_Window;
		Device* m_Device;
		std::unique_ptr<SwapChain>& m_SwapChain;
		std::unique_ptr<Pipeline>& m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer>& m_CommandBuffers;
		VkCommandPool m_CommandPool;
		VkDescriptorPool m_DescriptorPool;
		uint32_t m_MinImageCount;
	};

	class Renderer {
	public:
		Renderer();
		~Renderer();

		bool ShouldClose();
		void Update();


		const uint32_t GetWidth() { return m_Window.GetWidth(); }
		const uint32_t GetHeight() { return m_Window.GetHeight(); }

		RendererContext GetContext() {
			return {
				&m_Window,
				&m_Device,
				m_SwapChain,
				m_Pipeline,
				m_PipelineLayout,
				m_CommandBuffers,
				m_Device.getCommandPool(),
				m_DescriptorPool,
				m_MinImageCount
			};
		}

	private:
		Window m_Window{ "PitEngine", 800, 600};
		Device m_Device{ m_Window };
		std::unique_ptr<SwapChain> m_SwapChain;
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		VkDescriptorPool m_DescriptorPool;
		uint32_t m_MinImageCount;

		void _CreatePipelineLayout();
		void _CreatePipeline();
		void _CreateCommandBuffers();
		void _FreeCommandBuffers();
		void _DrawFrame();

		void _RecreateSwapChain();
		void _RecordCommandBuffer(int imageIndex);
		void _CreateDescriptorPool();

		std::unique_ptr<Mesh> m_TestMesh;
		void _LoadModels();
		void _Sierpinski(std::vector<Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);
	};
}