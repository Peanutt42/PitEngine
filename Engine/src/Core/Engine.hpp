#pragma once

#include "CoreInclude.hpp"
#include "EngineSettings.hpp"
#include "Layer.hpp"

namespace Pit {
	DEFINE_SIMPLE_EVENT(SaveConfigEvent);

	namespace ECS {
		class Scene;
	}
	namespace UI {
		class ImGuiLayer;
	}
	namespace Rendering {
		class Window;
	}
	class AudioSubmodule;
	class AssetManagmentSubmodule;
	class ECSSubmodule;
	class NetworkingSubmodule;
	class PhysicsSubmodule;
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
		static class ECSSubmodule* ECS();
		static class ScriptingSubmodule* Scripting();

		static void AddLayer(Layer* layer);
		static void AddOverlay(Layer* overlay);
		static void RemoveLayer(Layer* layer);
		static void RemoveOverlay(Layer* overlay);

		static SaveConfigEvent SaveConfigEvent;

		inline static Version Version = Version(0, 0, 1);

		static ECS::Scene* GetScene() { return s_Scene; }

		static Rendering::Window* GetWindow() { return s_Window; }

	private:
		static EngineSettings* s_Settings;

		inline static AudioSubmodule* s_AudioSubmodule = nullptr;
		inline static AntiCheatSubmodule* s_AntiCheatSubmodule = nullptr;
		inline static AssetManagmentSubmodule* s_AssetManagmentSubmodule = nullptr;
		inline static ECSSubmodule* s_ECSSubmodule = nullptr;
		inline static NetworkingSubmodule* s_NetworkingSubmodule = nullptr;
		inline static PhysicsSubmodule* s_PhysicsSubmodule = nullptr;
		inline static ScriptingSubmodule* s_ScriptingSubmodule = nullptr;

		inline static Rendering::Window* s_Window;

		inline static Array<Layer*> s_Layers;
		inline static size_t s_LayerInsertIndex;

		inline static UI::ImGuiLayer* s_ImGuiLayer;

		// TEMP!!!
		inline static ECS::Scene* s_Scene;
		
		static Array<std::function<void()>> s_MainThreadQueue;
		static std::mutex s_MainThreadQueueMutex;
		static void ExecuteMainThreadQueue();
	
		static std::ofstream s_InstanceLockFile;

		static bool s_Quit;
	};
}