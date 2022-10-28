#pragma once

#include "Core/CoreInclude.hpp"
#include "RenderingPipeline.hpp"
#include "RenderingDevice.hpp"
#include "RenderingSwapChain.hpp"
#include "Mesh.hpp"
#include "RenderingBuffer.hpp"
#include "Window.hpp"
#include "RenderingDescriptors.hpp"

namespace Pit::Rendering {
	DECLARE_EXTERN_PROFILE_STAT_FLOAT(RenderingRender, Rendering, "Rendering-Render");
	DECLARE_EXTERN_PROFILE_STAT_FLOAT(RenderingPresent, Rendering, "Rendering-Present");

	struct GlobalUBO {
		glm::mat4 projectionView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };  // w is intensity
		glm::vec3 lightPosition{ -1.f };
		alignas(16) glm::vec4 lightColor{ 1.f };  // w is light intensity
	};

	class Renderer {
	public:
		Renderer();
		~Renderer();

		bool ShouldClose();
		void Update();


		const uint32_t GetWidth() { return Window.GetWidth(); }
		const uint32_t GetHeight() { return Window.GetHeight(); }

		Window Window;

		Device Device;
		std::unique_ptr<Rendering::SwapChain> SwapChain;
		uint32_t ImageIndex = 0;
		int FrameIndex = 0;
		std::vector<VkCommandBuffer> CommandBuffers;
		std::unique_ptr<DescriptorPool> GlobalPool;
		std::unique_ptr<DescriptorSetLayout> GlobalSetLayout;
		std::vector<VkDescriptorSet> GlobalDescriptorSets = std::vector<VkDescriptorSet>(SwapChain::MAX_FRAMES_IN_FLIGHT);
		uint32_t MinImageCount;

		ScopeRef<Mesh>& TestMesh() { return m_TestMesh; }

		std::vector<std::unique_ptr<Buffer>> UBOBuffers = std::vector<std::unique_ptr<Buffer>>(SwapChain::MAX_FRAMES_IN_FLIGHT);


	private:
		void _CreateCommandBuffers();
		void _FreeCommandBuffers();
		void _BeginFrame();
		void _RenderFrame();
		void _PresentFrame();

		void _RecreateSwapChain();
		void _RecordCommandBuffer();

		ScopeRef<Mesh> m_TestMesh;
		void _LoadModels();
		void _Sierpinski(std::vector<Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);
	};
}