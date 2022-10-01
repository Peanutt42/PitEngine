#include "ECSWorld.hpp"
#include "ECSSystem.hpp"

using namespace Pit::ECS;

bool World::Init() {
	// Set Specs
	SetWorldSpecs(m_Specs);

	PIT_ENGINE_INFO("ECS-World '{}' succesfully initialized", m_Specs.name);
	PIT_ENGINE_INFO("[Specs]");
	PIT_ENGINE_INFO("[Name]: {}", m_Specs.name);

	return true;
}

bool World::Update(float DeltaTime) {
	if (Paused) return true;

	for (auto& system : m_Systems)
		system.Update(*this, DeltaTime);

	return true;
}

void World::Clear() {
	m_Registry.clear();
}

#pragma region Entity
entt::entity World::CreateEntity() {
	auto e = m_Registry.create();
	return e;
}

void World::DestroyEntity(entt::entity entity) {
	m_Registry.destroy(entity);
}
#pragma endregion

void World::SetWorldSpecs(WorldSpecs specs) {
	m_Specs = specs;
}