#pragma once

#include "CoreInclude.hpp"
#include "EngineSettings.hpp"

namespace Pit {
	DEFINE_SIMPLE_EVENT(UIRenderEvent);
	DEFINE_SIMPLE_EVENT(SaveConfigEvent);

	namespace ECS {
		class Scene;
	}
	class AudioSubmodule;
	class AssetManagmentSubmodule;
	class ECSSubmodule;
	class NetworkingSubmodule;
	class PhysicsSubmodule;
	class RenderingSubmodule;
	class AntiCheatSubmodule;
	class ScriptingSubmodule;
	
	/// <summary>
	/// Main class of the GameEngine.
	/// Used for initializing, updating and shutting down the Engine
	/// </summary>
	class Engine {
	public:
		static bool Init(EngineSettings& settings);
		static void Shutdown();
		static void ForceShutdown() { s_Quit = true; }
		static bool ShouldClose();
		static void Update();

		static const EngineSettings& GetSettings();

		static void SubmitToMainThread(std::function<void()> callback);

		static class AudioSubmodule* Audio();
		static class AssetManagmentSubmodule* AssetManagment();
		static class RenderingSubmodule* Rendering();
		static class ECSSubmodule* ECS();
		static class ScriptingSubmodule* Scripting();

		static UIRenderEvent UIRenderEvent;
		static SaveConfigEvent SaveConfigEvent;

		inline static Version Version = Version(0, 0, 1);

		static ECS::Scene* GetScene() { return s_Scene; }

	private:
		static EngineSettings* s_Settings;

		inline static AudioSubmodule* s_AudioSubmodule = nullptr;
		inline static AntiCheatSubmodule* s_AntiCheatSubmodule = nullptr;
		inline static AssetManagmentSubmodule* s_AssetManagmentSubmodule = nullptr;
		inline static ECSSubmodule* s_ECSSubmodule = nullptr;
		inline static NetworkingSubmodule* s_NetworkingSubmodule = nullptr;
		inline static PhysicsSubmodule* s_PhysicsSubmodule = nullptr;
		inline static RenderingSubmodule* s_RenderingSubmodule = nullptr;
		inline static ScriptingSubmodule* s_ScriptingSubmodule = nullptr;

		// TEMP!!!
		inline static ECS::Scene* s_Scene;
		
		static Array<std::function<void()>> s_MainThreadQueue;
		static std::mutex s_MainThreadQueueMutex;
		static void ExecuteMainThreadQueue();
	
		static std::ofstream s_InstanceLockFile;

		static bool s_Quit;
	};
}