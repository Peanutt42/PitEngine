#include "pch.hpp"
#include "Core/Engine.hpp"
#include "ECSSubmodule.hpp"
#include "ECSWorld.hpp"
#include "ECSSystem.hpp"
#include "ECSComponents.hpp"
#include "ECSEntityHandle.hpp"

namespace Pit::ECS {

	static void UpdateRendering() {
		PIT_PROFILE_FUNCTION("Pit::ECS::World::RenderingSystems");

		Pit::Engine::ECS()->GetEcsWorld().Update(SystemTopic::Render);
	}
	static void UpdateNetworking() {
		PIT_PROFILE_FUNCTION("Pit::ECS::World::NetworkingSystems");

		Pit::Engine::ECS()->GetEcsWorld().Update(SystemTopic::Networking);
	}
	static void UpdatePhysics() {
		PIT_PROFILE_FUNCTION("Pit::ECS::World::PhysicsSystems");

		Pit::Engine::ECS()->GetEcsWorld().Update(SystemTopic::Physic);
	}
	static void UpdateMain() {
		PIT_PROFILE_FUNCTION("Pit::ECS::World::MainSystems");

		Pit::Engine::ECS()->GetEcsWorld().Update(SystemTopic::General);
	}

	static void UpdateGameplay() {
		PIT_PROFILE_FUNCTION("Pit::ECS::World::GameplaySystems");

		Pit::Engine::ECS()->GetEcsWorld().Update(SystemTopic::Gameplay);
	}

	World::World(const WorldSpecs& specs) :
		m_Specs(specs) {}

	bool World::Init() {
		PIT_PROFILE_FUNCTION();

		// Set Specs
		SetWorldSpecs(m_Specs);

		Engine::NetworkingUpdateEvent += &UpdateNetworking;
		Engine::PhysicUpdateEvent += &UpdatePhysics;
		Engine::UpdateEvent += &UpdateMain;
		Engine::UpdateEvent += &UpdateGameplay;
		Engine::RenderEvent += &UpdateRendering;

		PIT_ENGINE_INFO(ECS, "ECS-World '{}' succesfully initialized", m_Specs.name);

		return true;
	}

	void World::Update(const SystemTopic topic) {
		PIT_PROFILE_FUNCTION();

		if (Paused) return;

		for (auto& system : m_Systems) {
			if (system.Topic == topic)
				system.Update(*this);
		}
	}

	void World::Clear() {
		PIT_PROFILE_FUNCTION();

		m_Registry.clear();
	}

	#pragma region Entity
	entt::entity World::CreateEntityID() {
		auto e = m_Registry.create();
		m_Registry.emplace<UUIDComponent>(e);
		m_Registry.emplace<NameComponent>(e);
		m_Registry.emplace<TransformComponent>(e);
		return e;
	}

	EntityHandle World::CreateEntity() {
		auto e = CreateEntityID();
		return { *this, e };
	}

	void World::DestroyEntity(entt::entity entity) {
		m_Registry.destroy(entity);
	}
	#pragma endregion

	void World::SetWorldSpecs(WorldSpecs specs) {
		m_Specs = specs;
	}
}