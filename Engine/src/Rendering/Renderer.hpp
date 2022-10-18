#pragma once

#include "Main/CoreInclude.hpp"
#include "Vulkan/VulkanPipeline.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanSwapChain.hpp"
#include "Vulkan/VulkanMesh.hpp"
#include "Window.hpp"

namespace Pit::Rendering {
	DECLARE_EXTERN_PROFILE_STAT_FLOAT(RenderingRender, Rendering, "Rendering-Render");
	DECLARE_EXTERN_PROFILE_STAT_FLOAT(RenderingPresent, Rendering, "Rendering-Present");

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
		ScopeRef<Rendering::SwapChain> SwapChain;
		ScopeRef<Rendering::Pipeline> Pipeline;
		VkPipelineLayout PipelineLayout;
		std::vector<VkCommandBuffer> CommandBuffers;
		VkDescriptorPool DescriptorPool;
		uint32_t MinImageCount;

	private:
		void _CreatePipelineLayout();
		void _CreatePipeline();
		void _CreateCommandBuffers();
		void _FreeCommandBuffers();
		void _BeginFrame();
		uint32_t _RenderFrame();
		void _PresentFrame(uint32_t imageIndex);

		void _RecreateSwapChain();
		void _RecordCommandBuffer(int imageIndex);
		void _CreateDescriptorPool();

		ScopeRef<Mesh> m_TestMesh;
		void _LoadModels();
		void _Sierpinski(std::vector<Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);
	};
}