#include "pch.hpp"
#include "Engine.hpp"
#include "Audio/AudioSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "Networking/NetworkingSubmodule.hpp"
#include "Physics/PhysicsSubmodule.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "Threading/JobSystem.hpp"
#include "Debug/vcrash.h"


using namespace Pit;
using namespace std::chrono;

InitEvent Engine::InitEvent;
NetworkingUpdateEvent Engine::NetworkingUpdateEvent;
PhysicUpdateEvent Engine::PhysicUpdateEvent;
PreUpdateEvent Engine::PreUpdateEvent;
UpdateEvent Engine::UpdateEvent;
PostUpdateEvent Engine::PostUpdateEvent;
RenderEvent Engine::RenderEvent;
UIRenderEvent Engine::UIRenderEvent;
OnWindowResizeEvent Engine::OnWindowResizeEvent;
ShutdownEvent Engine::ShutdownEvent;

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
	OPTICK_FRAME("MainThread - Init");

	PIT_PROFILE_FUNCTION();

	setup_crash_handler();

	try {
		s_CreateInfo = info;

		Debug::Logging::Init();

		for (const auto& arg : s_CreateInfo.ConsoleArgs) {
			if (arg == "-headless") s_CreateInfo.Headless = true;
		}

		PIT_ENGINE_INFO(General, "=== Initializing PIT::ENGINE ===");
		if (s_CreateInfo.Headless)
			PIT_ENGINE_INFO(General, " - Headless Mode");

		JobSystem::Initialize();

		if (!s_CreateInfo.Headless) {
			s_AudioSubmodule = new AudioSubmodule();
			s_AudioSubmodule->Init();
		}

		s_ECSSubmodule = new ECSSubmodule();
		s_ECSSubmodule->Init();

		if (!s_CreateInfo.Headless) {
			s_RenderingSubmodule = new RenderingSubmodule();
			s_RenderingSubmodule->Init();
		}
	
		s_NetworkingSubmodule = new NetworkingSubmodule();
		s_NetworkingSubmodule->Init();

		s_PhysicsSubmodule = new PhysicsSubmodule();
		s_PhysicsSubmodule->Init();

		InitEvent.Invoke();
	}
	CATCH_EXCEPTIONS();
}

void Engine::Shutdown() {
	OPTICK_FRAME("MainThread - Shutdown");

	PIT_PROFILE_FUNCTION();

	try {
		Engine::ShutdownEvent.Invoke();

		if (!s_CreateInfo.Headless) {
			s_RenderingSubmodule->Shutdown();
			delete s_RenderingSubmodule;
		}
		s_PhysicsSubmodule->Shutdown();
		delete s_PhysicsSubmodule;
		s_NetworkingSubmodule->Shutdown();
		delete s_NetworkingSubmodule;
		s_ECSSubmodule->Shutdown();
		delete s_ECSSubmodule;
		if (!s_CreateInfo.Headless) {
			s_AudioSubmodule->Shutdown();
			delete s_AudioSubmodule;
		}

		JobSystem::Shutdown();

		PIT_ENGINE_INFO(General, "=== PIT::ENGINE Shutdown ===");

		Debug::Logging::Shutdown();
	}
	CATCH_EXCEPTIONS();

	OPTICK_SHUTDOWN();
}

void Engine::Update() {
	OPTICK_FRAME("MainThread");

	PIT_PROFILE_FUNCTION();
	JobSystem::Execute([]() { for (int i = 0; i < 9999; i++) { String(std::to_string(i)); } });
	try {
		Time::SetFrame((Time::Frame() + 1) % 1000);

		static time_point<high_resolution_clock> lastUpdate;
		time_point<high_resolution_clock> now = high_resolution_clock::now();
		Time::SetDeltaTime(duration_cast<nanoseconds>(now - lastUpdate).count() * 0.000000001f);
		lastUpdate = now;

		Input::Update();

		s_NetworkingSubmodule->Update();
		NetworkingUpdateEvent.Invoke();
		
		s_PhysicsSubmodule->Update();
		PhysicUpdateEvent.Invoke();

		PreUpdateEvent.Invoke();
		UpdateEvent.Invoke();
		s_ECSSubmodule->Update();
		PostUpdateEvent.Invoke();

		if (!s_CreateInfo.Headless) {
			s_RenderingSubmodule->Update();
			
			s_AudioSubmodule->Update();
		}

	}
	CATCH_EXCEPTIONS();

}

bool Engine::ShouldClose() {
	if (s_Quit.load())
		return true;
	else if (!s_CreateInfo.Headless)
		return s_RenderingSubmodule->Window->ShouldClose();
	else
		return false;
}