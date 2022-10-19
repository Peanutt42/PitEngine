#include "Main/CoreInclude.hpp"
#include "Main/Engine.hpp"
#include "ECSWorld.hpp"
#include "ECSSystem.hpp"
#include "ECSComponents.hpp"

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
	// Set Specs
	SetWorldSpecs(m_Specs);

	Engine::NetworkingUpdateEvent += &UpdateNetworking;
	Engine::PhysicsUpdateEvent += &UpdatePhysics;
	Engine::UpdateEvent += &UpdateMain;
	Engine::RenderEvent += &UpdateRendering;

	PIT_ENGINE_INFO(Log::ECS, "ECS-World '{}' succesfully initialized", m_Specs.name);
	PIT_ENGINE_INFO(Log::ECS, "[Specs]");
	PIT_ENGINE_INFO(Log::ECS, "[Name]: {}", m_Specs.name);

	return true;
}

void World::Update(const SystemTopic topic) {
	if (Paused) return;

	if (topic == SystemTopic::Networking/*First off new frame*/) STAT_RESET(ECSUpdate);

	SCOPE_STAT_ADD(ECSUpdate);

	for (auto& system : m_Systems) {
		if (system.Topic == topic)
			system.Update(*this);
	}
}

void World::Clear() {
	m_Registry.clear();
}

#pragma region Entity
entt::entity World::CreateEntity() {
	auto e = m_Registry.create();
	m_Registry.emplace<UUIDComponent>(e);
	m_Registry.emplace<NameComponent>(e);
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