#pragma once

#include "CoreInclude.hpp"

namespace Pit {
	class AudioSubmodule;
	class ECSSubmodule;
	class NetworkingSubmodule;
	class PhysicsSubmodule;
	class RenderingSubmodule;

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

			CreateInfo(const int argc, const char* argv[], const String& windowName, bool windowToolbar, bool vsync, bool uiDocking, RenderingAPI rendereringApi = OpenGL_Api, int antiAliasing = 0)
				: WindowName(windowName), WindowToolbar(windowToolbar), VSync(vsync), UIDocking(uiDocking), RenderingApi(rendereringApi), AntiAliasing(antiAliasing) {

				for (int i = 0; i < argc; i++) ConsoleArgs.push_back(argv[i]);
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

		static Event<> InitEvent;
		static Event<> NetworkingUpdateEvent;
		static Event<> PhysicUpdateEvent;
		static Event<> PreUpdateEvent;
		static Event<> UpdateEvent;
		static Event<> PostUpdateEvent;
		static Event<> RenderEvent;
		static Event<> UIRenderEvent;
		static Event<> OnWindowResizeEvent;
		static Event<> ShutdownEvent;

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