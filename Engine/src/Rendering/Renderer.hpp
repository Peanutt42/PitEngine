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


		const uint32 GetWidth() { return Window.GetWidth(); }
		const uint32 GetHeight() { return Window.GetHeight(); }

		Window Window;

		Device Device;
		std::unique_ptr<Rendering::SwapChain> SwapChain;
		uint32 ImageIndex = 0;
		int FrameIndex = 0;
		Array<VkCommandBuffer> CommandBuffers;
		std::unique_ptr<DescriptorPool> GlobalPool;
		std::unique_ptr<DescriptorSetLayout> GlobalSetLayout;
		Array<VkDescriptorSet> GlobalDescriptorSets = Array<VkDescriptorSet>(SwapChain::MAX_FRAMES_IN_FLIGHT);
		uint32 MinImageCount;

		Array<std::unique_ptr<Buffer>> UBOBuffers = Array<std::unique_ptr<Buffer>>(SwapChain::MAX_FRAMES_IN_FLIGHT);

		ScopeRef<Mesh>& VaseMesh() { return m_VaseMesh; }
		ScopeRef<Mesh>& QuadMesh() { return m_QuadMesh; }

	private:
		void _CreateCommandBuffers();
		void _FreeCommandBuffers();
		void _BeginFrame();
		void _RenderFrame();
		void _PresentFrame();

		void _RecreateSwapChain();
		void _RecordCommandBuffer();

		void _LoadModels();
		ScopeRef<Mesh> m_VaseMesh;
		ScopeRef<Mesh> m_QuadMesh;
	};
}