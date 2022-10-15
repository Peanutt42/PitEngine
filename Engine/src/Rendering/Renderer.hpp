#pragma once

#include "Main/CoreInclude.hpp"
#include "Vulkan/VulkanPipeline.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanSwapChain.hpp"
#include "Vulkan/VulkanMesh.hpp"
#include "Window.hpp"

namespace Pit::Rendering {
	class Renderer {
	public:
		Renderer();
		~Renderer();

		bool ShouldClose();
		void Update();


		const uint32_t GetWidth() { return Window.GetWidth(); }
		const uint32_t GetHeight() { return Window.GetHeight(); }

		Window Window{ "PitEngine", 800, 600};
		Device Device{ Window };
		std::unique_ptr<Rendering::SwapChain> SwapChain;
		std::unique_ptr<Rendering::Pipeline> Pipeline;
		VkPipelineLayout PipelineLayout;
		std::vector<VkCommandBuffer> CommandBuffers;
		VkDescriptorPool DescriptorPool;
		uint32_t MinImageCount;

	private:
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