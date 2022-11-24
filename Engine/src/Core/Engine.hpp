#pragma once

#include "CoreInclude.hpp"
#include "EngineSettings.hpp"

namespace Pit {
	const static Version EngineVersion(0, 0, 1);

	class AudioSubmodule;
	class AssetManagmentSubmodule;
	class ECSSubmodule;
	class NetworkingSubmodule;
	class PhysicsSubmodule;
	class RenderingSubmodule;

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

		static AudioSubmodule* Audio() { return s_AudioSubmodule; }
		static AssetManagmentSubmodule* AssetManagment() { return s_AssetManagmentSubmodule; }
		static RenderingSubmodule* Rendering() { return s_RenderingSubmodule; }
		static ECSSubmodule* ECS() { return s_ECSSubmodule; }

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

		static AudioSubmodule* s_AudioSubmodule;
		static AssetManagmentSubmodule* s_AssetManagmentSubmodule;
		static ECSSubmodule* s_ECSSubmodule;
		static NetworkingSubmodule* s_NetworkingSubmodule;
		static PhysicsSubmodule* s_PhysicsSubmodule;
		static RenderingSubmodule* s_RenderingSubmodule;
		
		static std::atomic<bool> s_Quit;
	};
}