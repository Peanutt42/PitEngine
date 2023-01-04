#include "pch.hpp"
#include "Core/Engine.hpp"
#include "ECSSubmodule.hpp"

namespace Pit {

	void ECSSubmodule::Init() {
		PIT_PROFILE_FUNCTION();

		if (!m_ECSScene.Init())
			PIT_ENGINE_FATAL(ECS, "Error while initializing ECSWorld");

		for (int i = 0; i < 15; i++) {
			auto e = m_ECSScene.CreateEntity();
			e.GetComponent<ECS::TransformComponent>().position.x = i * 1.5f;
			e.GetComponent<ECS::NameComponent>().Name = "Entity" + std::to_string((uint32)e.GetID());
		}
	}

	void ECSSubmodule::Shutdown() {
		PIT_PROFILE_FUNCTION();

		m_ECSScene.Clear();
	}

	void ECSSubmodule::Update() {
		PIT_PROFILE_FUNCTION();

		Engine::PreUpdateEvent.Invoke();
		Engine::UpdateEvent.Invoke();
		Engine::PostUpdateEvent.Invoke();
	}

	void ECSSubmodule::ResetECSWorld() {
		m_ECSScene.Clear();
		String sceneName = m_ECSScene.GetName();
		m_ECSScene = ECS::Scene(sceneName);
		if (!m_ECSScene.Init())
			PIT_ENGINE_FATAL(ECS, "Error while initializing ECSWorld");
	}
	
	ECS::Scene& ECSSubmodule::GetEcsWorld() { return m_ECSScene; }
}