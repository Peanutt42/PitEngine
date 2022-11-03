#pragma once

#include "Audio/AudioSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "Networking/NetworkingSubmodule.hpp"
#include "Physics/PhysicsSubmodule.hpp"
#include "Rendering/RenderingSubmodule.hpp"

namespace Pit {
	class Engine {
	public:
		struct CreateInfo {
			Array<String> ConsoleArgs;
			String WindowName;
			bool WindowToolbar;
			bool VSync;
			bool UIDocking;

			CreateInfo(const int argc, const char* argv[], const String& windowName, bool windowToolbar, bool vsync, bool uiDocking) 
				: WindowName(windowName), WindowToolbar(windowToolbar), VSync(vsync), UIDocking(uiDocking) {

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