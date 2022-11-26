#include "pch.hpp"
#include "Engine.hpp"
#include "SubmoduleManager.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "Threading/JobSystem.hpp"
#include "Debug/vcrash.h"
#include "Debug/MemoryLeakDetector.hpp"


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

	SubmoduleManager*			Engine::s_SubmoduleManager = nullptr;

	std::ofstream				Engine::s_InstanceLockFile;

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

		Debug::MemoryLeakDetector::Init();

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

#define INSTANCE_LOCK_FILENAME "Instancelock.lock"
			if (s_Settings.OneInstanceOnly) {
				if (std::filesystem::exists(INSTANCE_LOCK_FILENAME)) PIT_ENGINE_FATAL(General, "One instance of the application is already opened!");
				s_InstanceLockFile.open(INSTANCE_LOCK_FILENAME);
			}

			JobSystem::Initialize();

			if (!s_Settings.Headless)
				Input::Init();

			s_SubmoduleManager->Init();

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

			s_SubmoduleManager->Shutdown();

			JobSystem::Shutdown();

			s_Settings.Serialize();
			SaveConfigEvent.Invoke();

			t.~ScopedTimer();

			PIT_ENGINE_INFO(General, "=== PIT::ENGINE Shutdown ===");

			Debug::Logging::Shutdown();

			Debug::MemoryLeakDetector::PrintOutPotentialMemLeaks();

			if (s_Settings.OneInstanceOnly) {
				s_InstanceLockFile.close();
				std::filesystem::remove({ INSTANCE_LOCK_FILENAME });
			}
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

			s_SubmoduleManager->Update();
		}
		CATCH_EXCEPTIONS();

	}

	bool Engine::ShouldClose() {
		if (s_Quit.load())
			return true;
		else if (!s_Settings.Headless)
			return s_SubmoduleManager->RenderingSubmodule->Window->ShouldClose();
		else
			return false;
	}

	AudioSubmodule* Engine::Audio() { return s_SubmoduleManager->AudioSubmodule; }
	AssetManagmentSubmodule* Engine::AssetManagment() { return s_SubmoduleManager->AssetManagmentSubmodule; }
	RenderingSubmodule* Engine::Rendering() { return s_SubmoduleManager->RenderingSubmodule; }
	ECSSubmodule* Engine::ECS() { return s_SubmoduleManager->ECSSubmodule; }
}