#include "Engineloop.hpp"
#include "Rendering/SimpleRenderSystem.hpp"
#include "Rendering/Vulkan/VulkanModel.hpp"
#include "ECS/Commons/ECSTransform2DComponent.hpp"
#include "ECS/Commons/ECSMeshComponent.hpp"

using namespace Pit;

Engineloop::Engineloop() {
	FileSystem::Init();

	_LoadExampleEntities();

	RendererSpecs rendererSpecs{};
	rendererSpecs.VSync = false;
	m_Renderer.SetSpecs(rendererSpecs);
}

Engineloop::~Engineloop() {
}

int Engineloop::Run() {
	SimpleRenderSystem simpleRenderSystem{ m_Device, m_Renderer.GetSwapChainRenderPass() };

	while (!m_Window.ShouldClose()) {
		//QUICK_SCOPE_PROFILE("Main thread update");

		glfwPollEvents();

		m_ECSSubsystem.Tick(0.16f);

		if (auto commandBuffer = m_Renderer.BeginFrame()) {
			m_Renderer.BeginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.RenderEntities(commandBuffer, m_ECSSubsystem.GetEcsWorld());
			m_Renderer.EndSwapChainRenderPass(commandBuffer);
			m_Renderer.EndFrame();
		}
	}

	vkDeviceWaitIdle(m_Device.device());

	return 0;
}


void Engineloop::_LoadExampleEntities() {
	std::vector<Vertex> vertices{
	  {{0.0f, -0.5f}},
	  {{0.5f, 0.5f}},
	  {{-0.5f, 0.5f}} };
	auto lveModel = std::make_shared<Model>(m_Device, vertices);

	auto world = m_ECSSubsystem.GetEcsWorld();
	auto e = world->CreateEntity();
	world->AddComponent<ECS::MeshComponent>(e, lveModel, glm::vec3(.1f, .8f, .1f));
	auto& transform = world->GetComponent<ECS::Transform2DComponent>(e);
	transform.scale = { 2.f, .5f };
	transform.rotation = .25f * glm::two_pi<float>();
}