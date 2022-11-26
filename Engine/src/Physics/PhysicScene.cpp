#include "pch.hpp"
#include "PhysicScene.hpp"

namespace Pit::Physics {

	Scene::Scene(physx::PxPhysics* physics, physx::PxDefaultCpuDispatcher* dispatcher) {
		PIT_PROFILE_FUNCTION();

		physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
		sceneDesc.gravity = m_Description.Gravity;
		sceneDesc.cpuDispatcher = dispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		m_Scene = physics->createScene(sceneDesc);
	}

	Scene::~Scene() {
	}

	void Scene::AddActor(physx::PxActor& actor, const physx::PxBVHStructure* bvhStructure) {
		m_Scene->addActor(actor, bvhStructure);
	}

	void Scene::Simulate(float delta) {
		PIT_PROFILE_FUNCTION();

		m_Scene->simulate(delta);
		m_Scene->fetchResults(true);
	}

	void Scene::UpdateDescription(const SceneDescription& newDescription) {
		PIT_PROFILE_FUNCTION();

		m_Description = newDescription;

		m_Scene->setGravity(m_Description.Gravity);
	}
}