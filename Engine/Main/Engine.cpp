#include "CoreInclude.hpp"
#include "Engine.hpp"
#include "ECS/Commons/ECSTransformComponent.hpp"
#include "ECS/Examples/ECSExampleMovement.hpp"
#include "Audio/SoundBuffer.hpp"
#include "Audio/SoundSource.hpp"
#include "Audio/AudioEngine.hpp"
#include "Audio/MusicBuffer.hpp"
#include <numeric>

using namespace Pit;
using namespace Audio;
using namespace std::chrono;

Engine* Engine::Instance = nullptr;

Engine::Engine() {
	FileSystem::Init();
	Debug::Logging::Init();
}

Engine::~Engine() {
	Debug::Logging::Shutdown();
}

void Engine::Init() {
	Instance = this;

	PIT_ENGINE_INFO(Log::General, "=== Initializing PIT::ENGINE ===");
	
	ECSSubsystem = new ECS::ECSSubsystem();
	ECSSubsystem->GetEcsWorld()->AddSystem<ECS::ExampleMovementSystem>();

	AudioEngine = new Audio::AudioEngine();
	AudioEngine->SetGain(0.2f);
	//sound1 = new MusicBuffer("C:/dev/cpp/PitEngine/Sandbox/audio/mc-virgins-yun-head-trap-anthem-official-lyric-video.mp3");
	//sound1Id = AudioEngine->AddMusicBuffer(sound1);

	Renderer = new Rendering::Renderer();
	UIRenderer = new Rendering::UI::Renderer(Renderer->m_Window->GetWindowHandle(), Renderer->m_Instance, Renderer->m_PhysicalDevice, Renderer->m_Device, Renderer->m_QueueFamily,
											 Renderer->m_Queue, Renderer->m_PipelineCache, Renderer->m_DescriptorPool, Renderer->m_MinImageCount, Renderer->m_ImageCount, Renderer->m_Allocator);
}

void Engine::Shutdown() {
	delete UIRenderer;
	delete Renderer;
	//delete sound1;
	delete AudioEngine;
	delete ECSSubsystem;

	PIT_ENGINE_INFO(Log::General, "=== PIT::ENGINE Shutdown ===");

	Debug::Logging::Shutdown();

	Instance = nullptr;
}

void Engine::Update() {
	static time_point<high_resolution_clock> lastUpdate;
	time_point<high_resolution_clock> now = high_resolution_clock::now();
	Time::DeltaTime = duration_cast<microseconds>(now - lastUpdate).count() / 1000000.0f;
	lastUpdate = now;

	ECSSubsystem->Tick();

	AudioEngine->Update();

	Renderer->Update();
}

bool Engine::ShouldClose() {
	if (m_ForceShutdown)
		return true;
	else
		return Renderer->ShouldClose();
}