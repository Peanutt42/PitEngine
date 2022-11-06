#include "pch.hpp"
#include "Core/Engine.hpp"
#include "ECSSubmodule.hpp"
#include "ECSWorld.hpp"
#include "ECSSystem.hpp"
#include "ECSComponents.hpp"
#include "ECSEntityHandle.hpp"

using namespace Pit::ECS;

static void UpdateRendering() {
	Pit::Engine::ECS()->GetEcsWorld().Update(SystemTopic::Render);
}
static void UpdateNetworking() {
	Pit::Engine::ECS()->GetEcsWorld().Update(SystemTopic::Networking);
}
static void UpdatePhysics() {
	Pit::Engine::ECS()->GetEcsWorld().Update(SystemTopic::Physic);
}
static void UpdateMain() {
	Pit::Engine::ECS()->GetEcsWorld().Update(SystemTopic::General);
	Pit::Engine::ECS()->GetEcsWorld().Update(SystemTopic::Gameplay);
}

bool World::Init() {
	PIT_PROFILE_FUNCTION();

	// Set Specs
	SetWorldSpecs(m_Specs);

	Engine::NetworkingUpdateEvent += &UpdateNetworking;
	Engine::PhysicUpdateEvent += &UpdatePhysics;
	Engine::UpdateEvent += &UpdateMain;
	Engine::RenderEvent += &UpdateRendering;

	PIT_ENGINE_INFO(Log::ECS, "ECS-World '{}' succesfully initialized", m_Specs.name);

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
	return { this, e };
}

void World::DestroyEntity(entt::entity entity) {
	m_Registry.destroy(entity);
}
#pragma endregion

void World::SetWorldSpecs(WorldSpecs specs) {
	m_Specs = specs;
}