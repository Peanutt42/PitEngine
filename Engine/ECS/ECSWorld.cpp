#include "CoreInclude.hpp"
#include "ECSWorld.hpp"
#include "ECSSystem.hpp"
#include "Commons/ECSTransformComponent.hpp"
#include "Commons/ECSEntityComponent.hpp"

using namespace Pit::ECS;

bool World::Init() {
	// Set Specs
	SetWorldSpecs(m_Specs);

	PIT_ENGINE_INFO(Log::ECS, "ECS-World '{}' succesfully initialized", m_Specs.name);
	PIT_ENGINE_INFO(Log::ECS, "[Specs]");
	PIT_ENGINE_INFO(Log::ECS, "[Name]: {}", m_Specs.name);

	return true;
}

bool World::Update() {
	if (Paused) return true;

	for (auto& system : m_Systems)
		system.Update(*this);

	return true;
}

void World::Clear() {
	m_Registry.clear();
}

#pragma region Entity
entt::entity World::CreateEntity() {
	auto e = m_Registry.create();
	m_Registry.emplace<EntityComponent>(e);
	m_Registry.emplace<TransformComponent>(e);
	return e;
}

void World::DestroyEntity(entt::entity entity) {
	m_Registry.destroy(entity);
}
#pragma endregion

void World::SetWorldSpecs(WorldSpecs specs) {
	m_Specs = specs;
}