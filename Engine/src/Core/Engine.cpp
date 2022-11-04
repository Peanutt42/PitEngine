#include "pch.hpp"
#include "Engine.hpp"
#include "Audio/AudioSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "Networking/NetworkingSubmodule.hpp"
#include "Physics/PhysicsSubmodule.hpp"
#include "Rendering/RenderingSubmodule.hpp"


using namespace Pit;
using namespace std::chrono;

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

Engine::CreateInfo		Engine::s_CreateInfo = Engine::CreateInfo(0, nullptr, "PitEngine-NullInfo", true, false, false);

AudioSubmodule*			Engine::s_AudioSubmodule = nullptr;
ECSSubmodule*			Engine::s_ECSSubmodule = nullptr;
NetworkingSubmodule*	Engine::s_NetworkingSubmodule = nullptr;
PhysicsSubmodule*		Engine::s_PhysicsSubmodule = nullptr;
RenderingSubmodule*		Engine::s_RenderingSubmodule = nullptr;

std::atomic<bool>		Engine::s_Quit = false;

#define CATCH_EXCEPTIONS() \
	catch (const std::exception& e) {											\
		std::cerr << "[Engine] Exception catched: " << e.what() << std::endl;	\
		return;																	\
	}																			\
	catch (...) {																\
		std::cerr << "[Engine] Exception catched!" << std::endl;				\
		return;																	\
	}

void Engine::Init(const CreateInfo& info) {
	PIT_PROFILE_BEGIN_SESSION("EngineInit", "EngineProfile-Init.json");

	try {
		Debug::Logging::Init();

		PIT_ENGINE_INFO(Log::General, "=== Initializing PIT::ENGINE ===");

		s_CreateInfo = info;

		s_AudioSubmodule = new AudioSubmodule();
		s_AudioSubmodule->Init();

		s_ECSSubmodule = new ECSSubmodule();
		s_ECSSubmodule->Init();

		s_RenderingSubmodule = new RenderingSubmodule();
		s_RenderingSubmodule->Init();
	
		s_NetworkingSubmodule = new NetworkingSubmodule();
		s_NetworkingSubmodule->Init();

		s_PhysicsSubmodule = new PhysicsSubmodule();
		s_PhysicsSubmodule->Init();

		Engine::InitEvent.Invoke();
	}
	CATCH_EXCEPTIONS();

	PIT_PROFILE_END_SESSION();
	
	PIT_PROFILE_BEGIN_SESSION("EngineRuntime", "EngineProfile-Runtime.json"); // Start Update-ProfileSession
}

void Engine::Shutdown() {
	PIT_PROFILE_END_SESSION(); // End Update-ProfileSession

	PIT_PROFILE_BEGIN_SESSION("EngineShutdown", "EngineProfile-Shutdown.json");

	try {
		Engine::ShutdownEvent.Invoke();

		s_RenderingSubmodule->Shutdown();
		delete s_RenderingSubmodule;
		s_PhysicsSubmodule->Shutdown();
		delete s_PhysicsSubmodule;
		s_NetworkingSubmodule->Shutdown();
		delete s_NetworkingSubmodule;
		s_ECSSubmodule->Shutdown();
		delete s_ECSSubmodule;
		s_AudioSubmodule->Shutdown();
		delete s_AudioSubmodule;

		PIT_ENGINE_INFO(Log::General, "=== PIT::ENGINE Shutdown ===");

		Debug::Logging::Shutdown();
	}
	CATCH_EXCEPTIONS();

	PIT_PROFILE_END_SESSION();
}

void Engine::Update() {
	try {
		Time::SetFrame((Time::Frame() + 1) % 1000);

		static time_point<high_resolution_clock> lastUpdate;
		time_point<high_resolution_clock> now = high_resolution_clock::now();
		Time::SetDeltaTime(duration_cast<nanoseconds>(now - lastUpdate).count() * 0.000000001f);
		lastUpdate = now;

		Input::Update();

		s_NetworkingSubmodule->Update();
		Engine::NetworkingUpdateEvent.Invoke();
		
		s_PhysicsSubmodule->Update();
		Engine::PhysicUpdateEvent.Invoke();

		Engine::PreUpdateEvent.Invoke();
		Engine::UpdateEvent.Invoke();
		s_ECSSubmodule->Update();

		s_RenderingSubmodule->Update();
		Engine::PostUpdateEvent.Invoke();

		s_AudioSubmodule->Update();
	}
	CATCH_EXCEPTIONS();

}

bool Engine::ShouldClose() {
	if (s_Quit.load())
		return true;
	else
		return s_RenderingSubmodule->Window->ShouldClose();
}