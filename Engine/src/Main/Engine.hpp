#pragma once

#include "ECS/ECSSubmodule.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "ECS/Examples/ECSExampleMovement.hpp"
#include "Rendering/Renderer.hpp"
#include "UI/UIRenderer.hpp"

namespace Pit {
	struct GlobalUbo {
		glm::mat4 projectionView{ 1.f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};
	class Engine {
	public:

		Engine();
		~Engine();
		
		void Init(), Shutdown();
		static void ForceShutdown() { m_Instance->m_Quit.store(true); }
		bool ShouldClose();
		void Update();

		static RenderingSubmodule* Rendering() { return m_Instance->m_RenderingSubmodule; }
		static ECSSubmodule* ECS() { return m_Instance->m_ECSSubmodule; }
		static UI::LayerManager* LayerManager() { return m_Instance->m_RenderingSubmodule->GetUIRenderer()->GetUILayerManager(); }


		static bool Exists() { return m_Instance; }


		static Event<> InitEvent;
		static Event<> TickEvent;
		static Event<> UpdateEvent;
		static Event<> RenderEvent;
		static Event<> UIRenderEvent;
		static Event<> OnWindowCloseEvent;
		static Event<> OnWindowResizeEvent;
		static Event<> ShutdownEvent;

	private:
		static Engine* m_Instance;

		ECSSubmodule* m_ECSSubmodule = nullptr;
		RenderingSubmodule* m_RenderingSubmodule = nullptr;
		
		std::atomic<bool> m_Quit = false;
	};
}