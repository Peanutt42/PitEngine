#include "pch.hpp"
#include "Engine.hpp"
#include "Audio/AudioSubmodule.hpp"
#include "AssetManagment/AssetManagmentSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "Networking/NetworkingSubmodule.hpp"
#include "Physics/PhysicsSubmodule.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "Threading/JobSystem.hpp"
#include "Debug/vcrash.h"


namespace Pit {

	InitEvent					Engine::InitEvent;
	NetworkingUpdateEvent		Engine::NetworkingUpdateEvent;
	PhysicUpdateEvent			Engine::PhysicUpdateEvent;
	PreUpdateEvent				Engine::PreUpdateEvent;
	UpdateEvent					Engine::UpdateEvent;
	PostUpdateEvent				Engine::PostUpdateEvent;
	RenderEvent					Engine::RenderEvent;
	UIRenderEvent				Engine::UIRenderEvent;
	OnWindowResizeEvent			Engine::OnWindowResizeEvent;
	SaveConfigEvent				Engine::SaveConfigEvent;
	ShutdownEvent				Engine::ShutdownEvent;

	EngineSettings				Engine::s_Settings = EngineSettings(0, nullptr, "NULL", "PitEngine-NullInfo", true, false);

	AudioSubmodule*				Engine::s_AudioSubmodule = nullptr;
	AssetManagmentSubmodule*	Engine::s_AssetManagmentSubmodule = nullptr;
	ECSSubmodule*				Engine::s_ECSSubmodule = nullptr;
	NetworkingSubmodule*		Engine::s_NetworkingSubmodule = nullptr;
	PhysicsSubmodule*			Engine::s_PhysicsSubmodule = nullptr;
	RenderingSubmodule*			Engine::s_RenderingSubmodule = nullptr;

	std::atomic<bool>			Engine::s_Quit = false;

#define CATCH_EXCEPTIONS() \
		catch ([[maybe_unused]] const std::exception& e) {											\
			PIT_ENGINE_FATAL(General, "[Engine] Exception catched: {}", e.what());	\
			return;																	\
		}																			\
		catch (...) {																\
			PIT_ENGINE_FATAL(General, "[Engine] Exception catched!");				\
			return;																	\
		}


	void Engine::Init(const EngineSettings& settings) {
		PIT_PROFILE_FRAME("MainThread");

		PIT_PROFILE_FUNCTION();

		ScopedTimer t("EngineInitTime");

		CrashHandler::Init();

		try {
			s_Settings = settings;

			Debug::Logging::Init();

			PIT_ENGINE_INFO(General, "=== Initializing PIT::ENGINE ===");
			PIT_ENGINE_INFO(General, " - Version: {}", EngineVersion);
			PIT_ENGINE_INFO(General, " - Vsync {}", s_Settings.VSync ? "On" : "Off");
			PIT_ENGINE_INFO(General, " - RenderingApi: {}", RenderingApiToString(s_Settings.RenderingApi));
			PIT_ENGINE_INFO(General, " - Antialiasing: {}", s_Settings.AntiAliasing);
			if (s_Settings.Headless)
				PIT_ENGINE_INFO(General, " - Headless Mode");

			JobSystem::Initialize();

			if (!s_Settings.Headless) {
				Input::Init();

				s_AudioSubmodule = new AudioSubmodule();
				s_AudioSubmodule->Init();
			}

			s_ECSSubmodule = new ECSSubmodule();
			s_ECSSubmodule->Init();

			if (!s_Settings.Headless) {
				s_RenderingSubmodule = new RenderingSubmodule();
				s_RenderingSubmodule->Init();
			}

			s_NetworkingSubmodule = new NetworkingSubmodule();
			s_NetworkingSubmodule->Init();

			s_PhysicsSubmodule = new PhysicsSubmodule();
			s_PhysicsSubmodule->Init();

			s_AssetManagmentSubmodule = new AssetManagmentSubmodule();
			s_AssetManagmentSubmodule->Init();

			InitEvent.Invoke();
		}
		CATCH_EXCEPTIONS();
	}

	void Engine::Shutdown() {
		PIT_PROFILE_FRAME("MainThread");

		PIT_PROFILE_FUNCTION();

		try {
			ScopedTimer t("EngineShutdownTime");

			Engine::ShutdownEvent.Invoke();

			s_AssetManagmentSubmodule->Shutdown();
			delete s_AssetManagmentSubmodule;

			if (!s_Settings.Headless) {
				s_RenderingSubmodule->Shutdown();
				delete s_RenderingSubmodule;
			}
			s_PhysicsSubmodule->Shutdown();
			delete s_PhysicsSubmodule;
			s_NetworkingSubmodule->Shutdown();
			delete s_NetworkingSubmodule;
			s_ECSSubmodule->Shutdown();
			delete s_ECSSubmodule;
			if (!s_Settings.Headless) {
				s_AudioSubmodule->Shutdown();
				delete s_AudioSubmodule;
			}

			JobSystem::Shutdown();

			s_Settings.Serialize();
			SaveConfigEvent.Invoke();

			t.~ScopedTimer();

			PIT_ENGINE_INFO(General, "=== PIT::ENGINE Shutdown ===");

			Debug::Logging::Shutdown();
		}
		CATCH_EXCEPTIONS();
	}

	void Engine::Update() {
		PIT_PROFILE_FRAME("MainThread");

		PIT_PROFILE_FUNCTION();

		try {
			Time::SetFrame((Time::Frame() + 1) % 1000);

			using namespace std::chrono;
			static time_point<high_resolution_clock> lastUpdate;
			time_point<high_resolution_clock> now = high_resolution_clock::now();
			Time::SetDeltaTime(duration_cast<nanoseconds>(now - lastUpdate).count() * .000000001f);
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

			if (!s_Settings.Headless) {
				s_RenderingSubmodule->Update();

				s_AudioSubmodule->Update();
			}

		}
		CATCH_EXCEPTIONS();

	}

	bool Engine::ShouldClose() {
		if (s_Quit.load())
			return true;
		else if (!s_Settings.Headless)
			return s_RenderingSubmodule->Window->ShouldClose();
		else
			return false;
	}
}