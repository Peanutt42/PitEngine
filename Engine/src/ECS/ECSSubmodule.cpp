#include "pch.hpp"
#include "Core/Engine.hpp"
#include "ECSSubmodule.hpp"

namespace Pit {

	void ECSSubmodule::Init() {
		PIT_PROFILE_FUNCTION();

		m_ECSWorld = ECS::World(m_ECSWorldSpecs);

		if (!m_ECSWorld.Init())
			PIT_ENGINE_FATAL(ECS, "Error while initializing ECSWorld");

		for (int i = 0; i < 15; i++) {
			auto e = m_ECSWorld.CreateEntity();
			e.GetComponent<ECS::TransformComponent>().position.x = i * 1.5f;
			e.GetComponent<ECS::NameComponent>().Name = "Entity" + std::to_string((uint32)e.GetID());
		}
	}

	void ECSSubmodule::Shutdown() {
		PIT_PROFILE_FUNCTION();

		m_ECSWorld.Clear();
	}

	void ECSSubmodule::Update() {
		PIT_PROFILE_FUNCTION();

		Engine::PreUpdateEvent.Invoke();
		Engine::UpdateEvent.Invoke();
		Engine::PostUpdateEvent.Invoke();
	}

	void ECSSubmodule::ResetECSWorld() {
		m_ECSWorld.Clear();
		m_ECSWorld = ECS::World(m_ECSWorldSpecs);
		if (!m_ECSWorld.Init())
			PIT_ENGINE_FATAL(ECS, "Error while initializing ECSWorld");
	}
	
	ECS::World& ECSSubmodule::GetEcsWorld() { return m_ECSWorld; }
}