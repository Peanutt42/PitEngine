#include "pch.hpp"
#include "Core/Engine.hpp"
#include "ECSSubmodule.hpp"
#include "Rendering/RenderingSubmodule.hpp"

namespace Pit {
	ECSSubmodule::ECSSubmodule() : m_ECSScene(String("MainECSScene"), nullptr) {}
	
	void ECSSubmodule::Init() {
		PIT_PROFILE_FUNCTION();

		if (!m_ECSScene.Init())
			PIT_ENGINE_FATAL(ECS, "Error while initializing ECSWorld");
	}

	void ECSSubmodule::Shutdown() {
		PIT_PROFILE_FUNCTION();

		m_ECSScene.Clear();
	}

	void ECSSubmodule::Update() {
		PIT_PROFILE_FUNCTION();

		m_ECSScene.SetCamera(Engine::Rendering()->Camera.Get());

		Engine::PreUpdateEvent.Invoke();
		Engine::UpdateEvent.Invoke();
		Engine::PostUpdateEvent.Invoke();
	}

	void ECSSubmodule::ResetECSWorld() {
		m_ECSScene.Clear();
		String sceneName = m_ECSScene.GetName();
		m_ECSScene = ECS::Scene(sceneName, Engine::Rendering()->Camera.Get());
		if (!m_ECSScene.Init())
			PIT_ENGINE_FATAL(ECS, "Error while initializing ECSWorld");
	}
	
	ECS::Scene& ECSSubmodule::GetEcsWorld() { return m_ECSScene; }
}