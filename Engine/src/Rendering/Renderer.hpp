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

#ifdef PIT_EDITOR
		Window Window{ "PitEngine - Editor", 800, 600};
#else
		Window Window{ "PitEngine - Sandbox", 800, 600};
#endif
		Device Device{ Window };
		std::unique_ptr<Rendering::SwapChain> SwapChain;
		uint32_t ImageIndex = 0;
		int FrameIndex = 0;
		std::vector<VkCommandBuffer> CommandBuffers;
		VkDescriptorPool DescriptorPool;
		uint32_t MinImageCount;

		ScopeRef<Mesh>& TestMesh() { return m_TestMesh; }

	private:
		void _CreateCommandBuffers();
		void _FreeCommandBuffers();
		void _BeginFrame();
		void _RenderFrame();
		void _PresentFrame();

		void _RecreateSwapChain();
		void _RecordCommandBuffer();
		void _CreateDescriptorPool();

		ScopeRef<Mesh> m_TestMesh;
		void _LoadModels();
		void _Sierpinski(std::vector<Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);
	};
}