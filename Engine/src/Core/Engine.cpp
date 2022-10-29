#include "pch.hpp"
#include "Engine.hpp"


using namespace Pit;
using namespace std::chrono;

Engine* Engine::m_Instance = nullptr;
Event<> Engine::InitEvent;
Event<> Engine::NetworkingUpdateEvent;
Event<> Engine::PhysicUpdateEvent;
Event<> Engine::PreUpdateEvent;
Event<> Engine::UpdateEvent;
Event<> Engine::PostUpdateEvent;
Event<> Engine::RenderEvent;
Event<> Engine::UIRenderEvent;
Event<> Engine::OnWindowResizeEvent;
Event<> Engine::ShutdownEvent;

Engine::Engine(const CreateInfo& createInfo) 
	: m_CreateInfo(createInfo) {

	Debug::Logging::Init();
}

Engine::~Engine() {
	Debug::Logging::Shutdown();
}

void Engine::Init() {
	try {
		m_Instance = this;

		PIT_ENGINE_INFO(Log::General, "=== Initializing PIT::ENGINE ===");

		m_AudioSubmodule = new AudioSubmodule();
		m_AudioSubmodule->Init();

		m_ECSSubmodule = new ECSSubmodule();
		m_ECSSubmodule->Init();

		m_RenderingSubmodule = new RenderingSubmodule();
		m_RenderingSubmodule->Init();

	
		m_NetworkingSubmodule = new NetworkingSubmodule();
		m_NetworkingSubmodule->Init();

		m_PhysicsSubmodule = new PhysicsSubmodule();
		m_PhysicsSubmodule->Init();

		m_AssetManagmentSubmodule = new AssetManagmentSubmodule();
		m_AssetManagmentSubmodule->Init();
		auto vase = m_ECSSubmodule->GetEcsWorld().CreateEntity();
		vase.AddComponent<ECS::MeshRendererComponent>(m_RenderingSubmodule->Renderer->VaseMesh().get());
		auto& vase_transform = vase.GetComponent<ECS::TransformComponent>();
		vase_transform.position = { .5f, .5f, 0.f };
		vase_transform.scale = { 3.f, 1.5f, 3.f };


		auto floor = m_ECSSubmodule->GetEcsWorld().CreateEntity();
		floor.AddComponent<ECS::MeshRendererComponent>(m_RenderingSubmodule->Renderer->QuadMesh().get());
		auto& floor_transform = floor.GetComponent<ECS::TransformComponent>();
		floor_transform.position = { 0.f, .5f, 0.f };
		floor_transform.scale = { 3.f, 1.f, 3.f };

		Engine::InitEvent.Invoke();
	}
	catch (const std::exception& e) {
		std::cerr << "[Engine] Exception catched: " << e.what() << std::endl;
		return;
	}
	catch (...) {
		std::cerr << "[Engine] Exception catched!" << std::endl;
		return ;
	}
}

void Engine::Shutdown() {
	try {
		Engine::ShutdownEvent.Invoke();

		m_AssetManagmentSubmodule->Shutdown();
		delete m_AssetManagmentSubmodule;
		m_RenderingSubmodule->Shutdown();
		delete m_RenderingSubmodule;
		m_PhysicsSubmodule->Shutdown();
		delete m_PhysicsSubmodule;
		m_NetworkingSubmodule->Shutdown();
		delete m_NetworkingSubmodule;
		m_ECSSubmodule->Shutdown();
		delete m_ECSSubmodule;
		m_AudioSubmodule->Shutdown();
		delete m_AudioSubmodule;

		PIT_ENGINE_INFO(Log::General, "=== PIT::ENGINE Shutdown ===");

		Debug::Logging::Shutdown();

		m_Instance = nullptr;
	}
	catch (const std::exception& e) {
		std::cerr << "[Engine] Exception catched: " << e.what() << std::endl;
		return;
	}
	catch (...) {
		std::cerr << "[Engine] Exception catched!" << std::endl;
		return;
	}
}

void Engine::Update() {
	try {
		Time::SetFrame((Time::Frame() + 1) % 1000);

		static time_point<high_resolution_clock> lastUpdate;
		time_point<high_resolution_clock> now = high_resolution_clock::now();
		Time::SetDeltaTime(duration_cast<nanoseconds>(now - lastUpdate).count() * 0.001f * 0.001f * 0.001f);
		lastUpdate = now;

		Input::Update();
		
		m_AssetManagmentSubmodule->Update();

		m_NetworkingSubmodule->Update();
		Engine::NetworkingUpdateEvent.Invoke();
		
		m_PhysicsSubmodule->Update();
		Engine::PhysicUpdateEvent.Invoke();

		Engine::PreUpdateEvent.Invoke();
		Engine::UpdateEvent.Invoke();
		m_ECSSubmodule->Update();

		m_RenderingSubmodule->Update();
		Engine::PostUpdateEvent.Invoke();

		m_AudioSubmodule->Update();
	}
	catch (const std::exception& e) {
		std::cerr << "[Engine] Exception catched: " << e.what() << std::endl;
		return;
	}
	catch (...) {
		std::cerr << "[Engine] Exception catched!" << std::endl;
		return;
	}
}

bool Engine::ShouldClose() {
	if (m_Quit.load())
		return true;
	else
		return m_RenderingSubmodule->Renderer->ShouldClose();
}