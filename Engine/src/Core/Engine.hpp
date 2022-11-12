#pragma once

#include "CoreInclude.hpp"

namespace Pit {
	const static Version EngineVersion(0, 0, 1);

	class AudioSubmodule;
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
	DEFINE_SIMPLE_EVENT(ShutdownEvent);
	class Engine {
	public:
		enum RenderingAPI {
			OpenGL_Api
		};

		struct CreateInfo {
			Array<String> ConsoleArgs;
			String WindowName;
			bool WindowToolbar;
			bool VSync;
			bool UIDocking;
			RenderingAPI RenderingApi;
			int AntiAliasing;
			bool Headless;

			CreateInfo(const int argc, const char* argv[], const String& windowName, bool windowToolbar, bool vsync, bool uiDocking, RenderingAPI rendereringApi = OpenGL_Api, int antiAliasing = 0, bool headless = false)
				: WindowName(windowName), WindowToolbar(windowToolbar), VSync(vsync), UIDocking(uiDocking), RenderingApi(rendereringApi), AntiAliasing(antiAliasing), Headless(headless) {

				ConsoleArgs.assign(argv, argv + argc);
			}
		};

		static void Init(const CreateInfo& info);
		static void Shutdown();
		static void ForceShutdown() { s_Quit.store(true); }
		static bool ShouldClose();
		static void Update();

		static const CreateInfo& GetInfo() { return s_CreateInfo; }

		static AudioSubmodule* Audio() { return s_AudioSubmodule; }
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
		static ShutdownEvent ShutdownEvent;

	private:
		static CreateInfo s_CreateInfo;

		static AudioSubmodule* s_AudioSubmodule;
		static ECSSubmodule* s_ECSSubmodule;
		static NetworkingSubmodule* s_NetworkingSubmodule;
		static PhysicsSubmodule* s_PhysicsSubmodule;
		static RenderingSubmodule* s_RenderingSubmodule;
		
		static std::atomic<bool> s_Quit;
	};
}