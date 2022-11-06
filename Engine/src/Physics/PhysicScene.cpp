#include "pch.hpp"
#include "PhysicScene.hpp"

using namespace Pit;
using namespace Physics;

Scene::Scene(physx::PxPhysics* physics, physx::PxDefaultCpuDispatcher* dispatcher) {
	physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = m_Description.Gravity;
	sceneDesc.cpuDispatcher = dispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	m_Scene = physics->createScene(sceneDesc);
}

void Scene::AddActor(physx::PxActor& actor, const physx::PxBVHStructure* bvhStructure) {
	m_Scene->addActor(actor, bvhStructure);
}

void Scene::Simulate(float delta) {
	m_Scene->simulate(delta);
	m_Scene->fetchResults(true);
}

void Scene::UpdateDescription(const SceneDescription& newDescription) {
	m_Description = newDescription;

	m_Scene->setGravity(m_Description.Gravity);
}
