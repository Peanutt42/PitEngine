#include "Main/CoreInclude.hpp"
#include "Engine.hpp"
#include "ECS/Examples/ECSExampleMovement.hpp"


using namespace Pit;
using namespace std::chrono;

Engine* Engine::m_Instance = nullptr;
Event<> Engine::InitEvent;
Event<> Engine::TickEvent;
Event<> Engine::UpdateEvent;
Event<> Engine::RenderEvent;
Event<> Engine::UIRenderEvent;
Event<> Engine::OnWindowCloseEvent;
Event<> Engine::OnWindowResizeEvent;
Event<> Engine::ShutdownEvent;

Engine::Engine() {
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

	m_RenderingSubmodule = new RenderingSubmodule();
	m_RenderingSubmodule->Init();
	
	Engine::InitEvent.Invoke();
}

void Engine::Shutdown() {
	Engine::ShutdownEvent.Invoke();

	m_RenderingSubmodule->Shutdown();
	delete m_RenderingSubmodule;
	m_ECSSubmodule->Shutdown();
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

	Engine::TickEvent.Invoke();
	Engine::UpdateEvent.Invoke();
	m_ECSSubmodule->Update();
	m_RenderingSubmodule->Update();
}

bool Engine::ShouldClose() {
	if (m_Quit.load())
		return true;
	else
		return m_RenderingSubmodule->GetRenderer()->ShouldClose();
}