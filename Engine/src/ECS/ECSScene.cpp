#include "pch.hpp"
#include "Core/Engine.hpp"
#include "ECSScene.hpp"
#include "ECSComponents.hpp"
#include "ECSEntityHandle.hpp"
#include "ECSSceneSerializer.hpp"

namespace Pit::ECS {

	Scene::Scene(const String& name) :
		m_Name(name), m_Camera({0, 0, 0.5f}) {}

	bool Scene::Init() {
		PIT_PROFILE_FUNCTION();

		return true;
	}

	void Scene::UpdateRuntime() {
		PIT_PROFILE_FUNCTION();
	}

	void Scene::UpdateEditor() {
		PIT_PROFILE_FUNCTION();
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