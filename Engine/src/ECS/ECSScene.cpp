#include "pch.hpp"
#include "Core/Engine.hpp"
#include "ECSSubmodule.hpp"
#include "ECSScene.hpp"
#include "ECSSystem.hpp"
#include "ECSComponents.hpp"
#include "ECSEntityHandle.hpp"
#include "ECSSceneSerializer.hpp"

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

	Scene::Scene(const String& name, Rendering::Camera* camera) :
		m_Name(name), m_Camera(camera) {}

	bool Scene::Init() {
		PIT_PROFILE_FUNCTION();

		Engine::NetworkingUpdateEvent += &UpdateNetworking;
		Engine::PhysicUpdateEvent += &UpdatePhysics;
		Engine::UpdateEvent += &UpdateMain;
		Engine::UpdateEvent += &UpdateGameplay;
		Engine::RenderEvent += &UpdateRendering;

		RegisterComponents(&m_Registry);

		return true;
	}

	void Scene::Update(const SystemTopic topic) {
		PIT_PROFILE_FUNCTION();

		if (Paused) return;

		for (auto& system : m_Systems) {
			if (system.Topic == topic)
				system.Update(*this);
		}
	}

	void Scene::Clear() {
		PIT_PROFILE_FUNCTION();

		m_Registry.clear();
	}

	#pragma region Entity
	entt::entity Scene::CreateEntityID() {
		auto e = m_Registry.create();
		m_Registry.emplace<NameComponent>(e);
		m_Registry.emplace<UUIDComponent>(e);
		m_Registry.emplace<TransformComponent>(e);
		return e;
	}

	EntityHandle Scene::CreateEntity() {
		auto e = CreateEntityID();
		return { this, e };
	}

	void Scene::DestroyEntity(entt::entity entity) {
		m_Registry.destroy(entity);
	}
	#pragma endregion

	void Scene::SetName(const String& name) { m_Name = name; }
	const String& Scene::GetName() const { return m_Name; }
}