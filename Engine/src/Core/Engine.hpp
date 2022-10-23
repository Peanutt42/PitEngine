#pragma once

#include "Audio/AudioSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "Networking/NetworkingSubmodule.hpp"
#include "Physics/PhysicsSubmodule.hpp"
#include "Rendering/RenderingSubmodule.hpp"

namespace Pit {
	struct GlobalUbo {
		glm::mat4 projectionView{ 1.f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};
	class Engine {
	public:
		struct CreateInfo {
			std::string WindowName;
			bool WindowToolbar;
		};

		Engine(const CreateInfo& createInfo);
		~Engine();
		
		void Init(), Shutdown();
		static void ForceShutdown() { m_Instance->m_Quit.store(true); }
		bool ShouldClose();
		void Update();

		static const CreateInfo& GetInfo() { return m_Instance->m_CreateInfo; }

		static RenderingSubmodule* Rendering() { return m_Instance->m_RenderingSubmodule; }
		static ECSSubmodule* ECS() { return m_Instance->m_ECSSubmodule; }
		static UI::LayerManager* LayerManager() { return m_Instance->m_RenderingSubmodule->UIRenderer->GetUILayerManager(); }


		static bool Exists() { return m_Instance; }

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
		static Engine* m_Instance;

		CreateInfo m_CreateInfo;

		AudioSubmodule* m_AudioSubmodule = nullptr;
		ECSSubmodule* m_ECSSubmodule = nullptr;
		NetworkingSubmodule* m_NetworkingSubmodule = nullptr;
		PhysicsSubmodule* m_PhysicsSubmodule = nullptr;
		RenderingSubmodule* m_RenderingSubmodule = nullptr;
		
		std::atomic<bool> m_Quit = false;
	};
}