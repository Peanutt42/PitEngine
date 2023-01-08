#include "pch.hpp"
#include "Engine.hpp"

#include "Audio/AudioSubmodule.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Window.hpp"
#include "UI/ImGuiLayer.hpp"
#include "Networking/NetworkingSubmodule.hpp"
#include "Physics/PhysicsSubmodule.hpp"
#include "AssetManagment/AssetManagmentSubmodule.hpp"
#include "AntiCheat/AntiCheatSubmodule.hpp"
#include "Scripting/ScriptingSubmodule.hpp"

#include "ECS/ECSScene.hpp"
#include "Threading/JobSystem.hpp"
#include "Debug/vcrash.h"


namespace Pit {
	SaveConfigEvent					Engine::SaveConfigEvent;

	EngineSettings*					Engine::s_Settings = nullptr;

	std::ofstream					Engine::s_InstanceLockFile;

	Array<std::function<void()>>	Engine::s_MainThreadQueue;
	std::mutex						Engine::s_MainThreadQueueMutex;

	bool							Engine::s_Quit = false;

#define CATCH_EXCEPTIONS() \
		catch ([[maybe_unused]] const std::bad_alloc& e)	{						\
			PIT_ENGINE_FATAL(Memory, "[Engine] Bad Allocation catched: {}", e.what());	\
		}																			\
		catch ([[maybe_unused]] const std::exception& e) {							\
			PIT_ENGINE_FATAL(General, "[Engine] Exception catched: {}", e.what());	\
		}																			\
		catch (...) {																\
			PIT_ENGINE_FATAL(General, "[Engine] Exception catched!");				\
		}


	bool Engine::Init(EngineSettings& settings) {
		PIT_PROFILE_FRAME("MainThread");

		PIT_PROFILE_FUNCTION();

		CrashHandler::Init();

		try {
			s_Settings = &settings;

			s_Settings->Deserialize(FileSystem::GetConfigDir() + s_Settings->Prefix + "Config.ini");

			for (const auto& arg : s_Settings->ConsoleArgs) {
				if (arg == "--version" || arg == "-v") {
					std::cout << "PitEngine Game Engine\n"
								 "Github page: https://github.com/Peanutt42/PitEngine\n"
								 "Version: " << Engine::Version;
					ForceShutdown();
					return false;
				}
			}

#define INSTANCE_LOCK_FILENAME "Instancelock.lock"
			if (s_Settings->OneInstanceOnly) {
				if (std::filesystem::exists(INSTANCE_LOCK_FILENAME)) {
					s_InstanceLockFile.open(INSTANCE_LOCK_FILENAME);
					if (!s_InstanceLockFile.is_open()) {
						MessagePrompts::InfoMessage(L"Application is already opened", L"One instance of the application is already opened!");
						return false;
					}
				}
				else
					s_InstanceLockFile.open(INSTANCE_LOCK_FILENAME);
			}

			ScopedTimer t("EngineInitTime");

			Debug::Logging::Init();

			PIT_ENGINE_INFO(General, "=== Initializing PIT::ENGINE ===");
			PIT_ENGINE_INFO(General, " - Version: {}", Engine::Version);
			PIT_ENGINE_INFO(General, " - Vsync {}", s_Settings->VSync ? "On" : "Off");
			PIT_ENGINE_INFO(General, " - RenderingApi: {}", RenderingApiToString(s_Settings->RenderingApi));
			PIT_ENGINE_INFO(General, " - Antialiasing: {}", s_Settings->AntiAliasing);
			PIT_ENGINE_INFO(General, " - MaxFps: {}", s_Settings->MaxFps);
			if (s_Settings->Headless)
				PIT_ENGINE_INFO(General, " - Headless Mode");

			JobSystem::Initialize();

			if (!s_Settings->Headless)
				Input::Init();

			s_Scene = new ECS::Scene("MainScene");

			s_AntiCheatSubmodule = new Pit::AntiCheatSubmodule();
			s_AntiCheatSubmodule->Init();


			if (!Engine::GetSettings().Headless) {
				s_AudioSubmodule = new Pit::AudioSubmodule();
				s_AudioSubmodule->Init();
			}

			s_ScriptingSubmodule = new Pit::ScriptingSubmodule();
			s_ScriptingSubmodule->Init();

			if (!Engine::GetSettings().Headless) {
				s_Window = new Rendering::Window(s_Settings->WindowName, 1920, 1080, false);
				Rendering::Renderer::Init();

				s_ImGuiLayer = new UI::ImGuiLayer();
				AddOverlay(s_ImGuiLayer);
			}

			s_NetworkingSubmodule = new Pit::NetworkingSubmodule();
			s_NetworkingSubmodule->Init();

			s_PhysicsSubmodule = new Pit::PhysicsSubmodule();
			s_PhysicsSubmodule->Init();

			s_AssetManagmentSubmodule = new Pit::AssetManagmentSubmodule();
			s_AssetManagmentSubmodule->Init();
		}
		CATCH_EXCEPTIONS();

		return true;
	}

	#if DEBUG
	static void waitForOptickToCollectShutdownTimings() {
		for (int i = 0; i < 3; i++) {
			PIT_PROFILE_FRAME("MainThread");
			PIT_PROFILE_FUNCTION("END");
			std::this_thread::sleep_for(1s);
		}
	}
	#endif
	void Engine::Shutdown() {
		PIT_PROFILE_FRAME("MainThread");

		PIT_PROFILE_FUNCTION();

		s_Quit = true;

		try {
			{
				for (Layer* layer : s_Layers) {
					layer->OnDestroy();
					delete layer;
				}
				ScopedTimer t("EngineShutdownTime");
				s_Scene->Clear();
				delete s_Scene;
				s_AssetManagmentSubmodule->Shutdown();
				delete s_AssetManagmentSubmodule;

				if (!Engine::GetSettings().Headless) {
					Rendering::Renderer::Shutdown();
					delete s_Window;
				}
				s_PhysicsSubmodule->Shutdown();
				delete s_PhysicsSubmodule;
				s_NetworkingSubmodule->Shutdown();
				delete s_NetworkingSubmodule;
				s_ScriptingSubmodule->Shutdown();
				delete s_ScriptingSubmodule;
				if (!Engine::GetSettings().Headless) {
					s_AudioSubmodule->Shutdown();
					delete s_AudioSubmodule;
				}
				s_AntiCheatSubmodule->Shutdown();
				delete s_AntiCheatSubmodule;

				JobSystem::Shutdown();

				s_Settings->Serialize(FileSystem::GetConfigDir() + s_Settings->Prefix + "Config.ini");
				SaveConfigEvent.Invoke();
			}

			PIT_ENGINE_INFO(General, "=== PIT::ENGINE Shutdown ===");

			Debug::Logging::Shutdown();

			if (s_Settings->OneInstanceOnly && s_InstanceLockFile) {
				s_InstanceLockFile.close();
				std::filesystem::remove({ INSTANCE_LOCK_FILENAME });
			}
		}
		CATCH_EXCEPTIONS();

		#if DEBUG
		waitForOptickToCollectShutdownTimings();
		#endif
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

			ExecuteMainThreadQueue();

			Input::Update();

			s_AntiCheatSubmodule->Update();

			s_NetworkingSubmodule->Update();

			s_PhysicsSubmodule->Update();

			s_ScriptingSubmodule->Update();

			{
				PIT_PROFILE_FUNCTION("Pit::Engine::Update::Layers::OnUpdate");
				for (Layer* layer : s_Layers) layer->OnUpdate();
			}

			if (!Engine::GetSettings().Headless) {
				Rendering::Renderer::Begin();
				{
					PIT_PROFILE_FUNCTION("Pit::Engine::Update::Layers::OnRender");
					for (Layer* layer : s_Layers) layer->OnRender();
				}
				Rendering::Renderer::End();

				s_AudioSubmodule->Update();
			}
			s_ImGuiLayer->Begin();
			{
				PIT_PROFILE_FUNCTION("Pit::Engine::Update::Layers::OnUIRender");
				for (Layer* layer : s_Layers) layer->OnUIRender();
			}
			s_ImGuiLayer->End();

			s_Window->Update();

			// Limit the Fps
			if (s_Settings->MaxFps > 0) {
				float updateTime = duration_cast<nanoseconds>(high_resolution_clock::now() - now).count() * .000000001f;
				float timeLeft = (1.f / s_Settings->MaxFps) - updateTime;
				if (timeLeft > 0.f) {
					PIT_PROFILE_FUNCTION("Pit::Engine::MaxFpsLimitWait");
					Time::MicroSleep(Cast<uint64>(floor(timeLeft * 1000 * 1000))); // Wait for microseconds
				}
			}
		}
		CATCH_EXCEPTIONS();

	}

	const EngineSettings& Engine::GetSettings() { return *s_Settings; }

	bool Engine::ShouldClose() {
		if (s_Quit)
			return true;
		else if (!s_Settings->Headless)
			return s_Window->ShouldClose();
		else
			return false;
	}


	void Engine::SubmitToMainThread(std::function<void()> callback) {
		std::scoped_lock lock(s_MainThreadQueueMutex);
		s_MainThreadQueue.emplace_back(callback);
	}

	void Engine::ExecuteMainThreadQueue() {
		for (auto& callback : s_MainThreadQueue) callback();
		s_MainThreadQueue.clear();
	}


	AudioSubmodule* Engine::Audio()						{ return s_AudioSubmodule; }
	AssetManagmentSubmodule* Engine::AssetManagment()	{ return s_AssetManagmentSubmodule;  }
	ECSSubmodule* Engine::ECS()							{ return s_ECSSubmodule;  }
	ScriptingSubmodule* Engine::Scripting() { return s_ScriptingSubmodule; }
	
	void Engine::AddLayer(Layer* layer) {
		s_Layers.emplace(s_Layers.begin() + s_LayerInsertIndex, layer);
		s_LayerInsertIndex++;
		layer->OnCreate();
	}
	void Engine::AddOverlay(Layer* overlay) {
		s_Layers.emplace_back(overlay);
		overlay->OnCreate();
	}
	void Engine::RemoveLayer(Layer* layer) {
		auto it = std::find(s_Layers.begin(), s_Layers.begin() + s_LayerInsertIndex, layer);
		if (it != s_Layers.begin() + s_LayerInsertIndex) {
			layer->OnDestroy();
			delete layer;
			s_Layers.erase(it);
			s_LayerInsertIndex--;
		}
	}
	void Engine::RemoveOverlay(Layer* overlay) {
		auto it = std::find(s_Layers.begin() + s_LayerInsertIndex, s_Layers.end(), overlay);
		if (it != s_Layers.end()) {
			overlay->OnDestroy();
			delete overlay;
			s_Layers.erase(it);
		}
	}
}