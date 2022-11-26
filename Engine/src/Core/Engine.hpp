#pragma once

#include "CoreInclude.hpp"
#include "EngineSettings.hpp"

namespace Pit {
	const static Version EngineVersion(0, 0, 1);

	DEFINE_SIMPLE_EVENT(InitEvent);
	DEFINE_SIMPLE_EVENT(NetworkingUpdateEvent);
	DEFINE_SIMPLE_EVENT(PhysicUpdateEvent);
	DEFINE_SIMPLE_EVENT(PreUpdateEvent);
	DEFINE_SIMPLE_EVENT(UpdateEvent);
	DEFINE_SIMPLE_EVENT(PostUpdateEvent);
	DEFINE_SIMPLE_EVENT(RenderEvent);
	DEFINE_SIMPLE_EVENT(UIRenderEvent);
	DEFINE_SIMPLE_EVENT(OnWindowResizeEvent);
	DEFINE_SIMPLE_EVENT(SaveConfigEvent);
	DEFINE_SIMPLE_EVENT(ShutdownEvent);
	
	/// <summary>
	/// Main class of the GameEngine.
	/// Used for initializing, updating and shutting down the Engine
	/// </summary>
	class Engine {
	public:
		static void Init(const EngineSettings& settings);
		static void Shutdown();
		static void ForceShutdown() { s_Quit.store(true); }
		static bool ShouldClose();
		static void Update();

		static const EngineSettings& GetSettings() { return s_Settings; }

		static class AudioSubmodule* Audio();
		static class AssetManagmentSubmodule* AssetManagment();
		static class RenderingSubmodule* Rendering();
		static class ECSSubmodule* ECS();

		static InitEvent InitEvent;
		static NetworkingUpdateEvent NetworkingUpdateEvent;
		static PhysicUpdateEvent PhysicUpdateEvent;
		static PreUpdateEvent PreUpdateEvent;
		static UpdateEvent UpdateEvent;
		static PostUpdateEvent PostUpdateEvent;
		static RenderEvent RenderEvent;
		static UIRenderEvent UIRenderEvent;
		static OnWindowResizeEvent OnWindowResizeEvent;
		static SaveConfigEvent SaveConfigEvent;
		static ShutdownEvent ShutdownEvent;

	private:
		static EngineSettings s_Settings;

		static class SubmoduleManager* s_SubmoduleManager;
		
		static std::ofstream s_InstanceLockFile;

		static std::atomic<bool> s_Quit;
	};
}