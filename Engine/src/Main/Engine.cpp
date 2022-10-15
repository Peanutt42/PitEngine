#include "Main/CoreInclude.hpp"
#include "Engine.hpp"
#include "ECS/Examples/ECSExampleMovement.hpp"


using namespace Pit;
using namespace Audio;
using namespace std::chrono;

Engine* Engine::m_Instance = nullptr;

Engine::Engine() {
	FileSystem::Init();
	Debug::Logging::Init();
}

Engine::~Engine() {
	Debug::Logging::Shutdown();
}

void Engine::Init() {
	m_Instance = this;

	PIT_ENGINE_INFO(Log::General, "=== Initializing PIT::ENGINE ===");
	
	m_ECSSubmodule = new ECSSubmodule();
	m_ECSSubmodule->Init();
	m_ECSSubmodule->GetEcsWorld().AddSystem<ECS::ExampleMovementSystem>();

	m_AudioSubmodule = new AudioSubmodule();
	m_AudioSubmodule->Init();
	m_RenderingSubmodule = new RenderingSubmodule();
	m_RenderingSubmodule->Init();
}

void Engine::Shutdown() {
	m_RenderingSubmodule->Shutdown();
	delete m_RenderingSubmodule;
	m_AudioSubmodule->Shutdown();;
	delete m_AudioSubmodule;
	m_ECSSubmodule->Shutdown();;
	delete m_ECSSubmodule;

	PIT_ENGINE_INFO(Log::General, "=== PIT::ENGINE Shutdown ===");

	Debug::Logging::Shutdown();

	m_Instance = nullptr;
}

void Engine::Update() {
	Time::SetFrame((Time::Frame() + 1) % 1000);

	static time_point<high_resolution_clock> lastUpdate;
	time_point<high_resolution_clock> now = high_resolution_clock::now();
	Time::SetDeltaTime(duration_cast<microseconds>(now - lastUpdate).count() / 1000000.f);
	lastUpdate = now;

	m_ECSSubmodule->Update();
	m_AudioSubmodule->Update();
	m_RenderingSubmodule->Update();
}

bool Engine::ShouldClose() {
	if (m_Quit.load())
		return true;
	else
		return m_RenderingSubmodule->GetRenderer()->ShouldClose();
}