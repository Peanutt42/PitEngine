#include "Engineloop.hpp"
#include "Rendering/SimpleRenderSystem.hpp"
#include "Rendering/Vulkan/VulkanModel.hpp"
#include "ECS/Commons/ECSTransformComponent.hpp"
#include "ECS/Commons/ECSMeshComponent.hpp"
#include "Audio/SoundBuffer.hpp"
#include "Audio/SoundSource.hpp"
#include "Audio/AudioEngine.hpp"
#include "Audio/MusicBuffer.hpp"

using namespace Pit;
using namespace Audio;
using namespace std::chrono;

Engineloop* Engineloop::Instance = nullptr;

Engineloop::Engineloop() {
	Instance = this;

	PIT_ENGINE_INFO(" === Initializing PIT::ENGINE === ");

	Window = new Rendering::Window(800, 600, "PitEngine");

	Device = new Rendering::Device(*Window);
	
	ECSSubsystem = new ECS::ECSSubsystem();
	_LoadExampleEntities();

	Renderer = new Rendering::Renderer(*Device, *Window, ECSSubsystem->GetEcsWorld());
	Rendering::RendererSpecs rendererSpecs{};
	rendererSpecs.VSync = false;
	Renderer->SetSpecs(rendererSpecs);

	AudioEngine::get();

	sound1 = new MusicBuffer((FileSystem::GetSandboxDir() + "/audio/wav_2mb.wav").c_str());
	sound1->Play();
	
}

Engineloop::~Engineloop() {
	vkDeviceWaitIdle(Device->device());

	delete SimpleRenderSystem;
	delete Renderer;
	delete ECSSubsystem;
	delete sound1;
	delete Device;
	delete Window;

	PIT_ENGINE_INFO(" === PIT::ENGINE Shutdown === ");

	Debug::Logging::Shutdown();

	Instance = nullptr;
}

int Engineloop::Run() {
	SimpleRenderSystem = new Rendering::SimpleRenderSystem(*Device, Renderer->GetSwapChainRenderPass());

	while (!Window->ShouldClose()) {
		Update();
	}

	return 0;
}

static time_point<high_resolution_clock> last_update;
void Engineloop::Update() {
	time_point<high_resolution_clock> now = high_resolution_clock::now();
	Time::DeltaTime = duration_cast<microseconds>(now - last_update).count() / 1000000.0f;
	last_update = now;
	//PIT_ENGINE_INFO("{0}, {1}, {2}", Time::DeltaTime, 1 / Time::DeltaTime * 1, Time::DeltaTime * 1000);
	
	glfwPollEvents();

	sound1->UpdateBufferStream();

	ECSSubsystem->Tick();

	if (auto commandBuffer = Renderer->BeginFrame()) {
		Renderer->BeginSwapChainRenderPass(commandBuffer);
		SimpleRenderSystem->RenderEntities(commandBuffer, ECSSubsystem->GetEcsWorld());
		Renderer->EndSwapChainRenderPass(commandBuffer);
		Renderer->EndFrame();
	}	
}




std::unique_ptr<Rendering::Model> createCubeModel(Rendering::Device& device, glm::vec3 offset) {
	std::vector<Rendering::Vertex> vertices{

		// left face (white)
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

		// right face (yellow)
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

		// top face (orange, remember y axis points down)
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

		// bottom face (red)
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

		// nose face (blue)
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

		// tail face (green)
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

	};
	for (auto& v : vertices) {
		v.position += offset;
	}
	return std::make_unique<Rendering::Model>(device, vertices);
}

void Engineloop::_LoadExampleEntities() {
	std::shared_ptr<Rendering::Model> Model = createCubeModel(*Device, { 0.f, 0.f, 0.f });

	auto ecsworld = ECSSubsystem->GetEcsWorld();
	auto cube = ecsworld->CreateEntity();
	ecsworld->AddComponent<ECS::MeshComponent>(cube, Model);
	auto& transform = ecsworld->GetComponent<ECS::TransformComponent>(cube);
	transform.position = { 0.f, 0.f, .5f };
	transform.scale = { .5f, .5f, .5f };
}