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
		catch ([[maybe_unused]] const std::exception& e) {							\
			PIT_ENGINE_FATAL(General, "[Engine] Exception catched: {}", e.what());	\
		}																			\
		catch (...) {																\
			PIT_ENGINE_FATAL(General, "[Engine] Exception catched!");				\
		}


	bool Engine::Init(const EngineSettings& settings) {
		PIT_PROFILE_FRAME("MainThread");

		PIT_PROFILE_FUNCTION();

		CrashHandler::Init();

		Debug::MemoryLeakDetector::Init();

		try {
			s_Settings = settings;

			for (auto& arg : s_Settings.ConsoleArgs) {
				if (arg == "--version" || arg == "-v") {
					std::cout << "PitEngine Game Engine\n"
								 "Github page: https://github.com/Peanutt42/PitEngine\n"
								 "Version: " << EngineVersion;
					ForceShutdown();
					return false;
				}
			}

#define INSTANCE_LOCK_FILENAME "Instancelock.lock"
			if (s_Settings.OneInstanceOnly) {
				if (std::filesystem::exists(INSTANCE_LOCK_FILENAME)) {
					MessagePrompts::InfoMessage(L"Application is already opened", L"One instance of the application is already opened!");
					return false;
				}
				s_InstanceLockFile.open(INSTANCE_LOCK_FILENAME);
			}

			ScopedTimer t("EngineInitTime");

			Debug::Logging::Init();

			PIT_ENGINE_INFO(General, "=== Initializing PIT::ENGINE ===");
			PIT_ENGINE_INFO(General, " - Version: {}", EngineVersion);
			PIT_ENGINE_INFO(General, " - Vsync {}", s_Settings.VSync ? "On" : "Off");
			PIT_ENGINE_INFO(General, " - RenderingApi: {}", RenderingApiToString(s_Settings.RenderingApi));
			PIT_ENGINE_INFO(General, " - Antialiasing: {}", s_Settings.AntiAliasing);
			PIT_ENGINE_INFO(General, " - MaxFps: {}", s_Settings.MaxFps);
			if (s_Settings.Headless)
				PIT_ENGINE_INFO(General, " - Headless Mode");

			JobSystem::Initialize();

			if (!s_Settings.Headless)
				Input::Init();

			s_SubmoduleManager = new SubmoduleManager();
			s_SubmoduleManager->Init();

			InitEvent.Invoke();
		}
		CATCH_EXCEPTIONS();

		return true;
	}

	void Engine::Shutdown() {
		PIT_PROFILE_FRAME("MainThread");

		PIT_PROFILE_FUNCTION();

		try {
			{
				ScopedTimer t("EngineShutdownTime");
				Engine::ShutdownEvent.Invoke();

				s_SubmoduleManager->Shutdown();

				JobSystem::Shutdown();

				s_Settings.Serialize();
				SaveConfigEvent.Invoke();
			}

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

			// Limit the Fps
			if (s_Settings.MaxFps != -1 && s_Settings.MaxFps != 0) {
				float updateTime = duration_cast<nanoseconds>(high_resolution_clock::now() - now).count() * .000000001f;
				float timeLeft = (1.f / s_Settings.MaxFps) - updateTime;
				if (timeLeft > 0.f) {
					PIT_PROFILE_FUNCTION("Pit::Engine::MaxFpsLimitWait");
					Time::MicroSleep(Cast<uint64>(floor(timeLeft * 1000 * 1000))); // Wait for microseconds
				}
			}
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