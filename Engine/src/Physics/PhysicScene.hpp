#pragma once

#include "Core/CoreInclude.hpp"
#include <PxPhysicsAPI.h>

namespace Pit::Physics {
	struct SceneDescription {
		physx::PxVec3 Gravity = physx::PxVec3(0, -9.81f, 0);
	};

	class Scene {
	public:
		Scene(physx::PxPhysics* physics, physx::PxDefaultCpuDispatcher* dispatcher);

		void AddActor(physx::PxActor& actor, const physx::PxBVHStructure* bvhStructure = nullptr);

		void Simulate(float delta);

		const SceneDescription& GetDescription() { return m_Description; }
		void UpdateDescription(const SceneDescription& newDescription);

		physx::PxScene* GetNative() { return m_Scene; }

	private:
		SceneDescription m_Description;

		physx::PxScene* m_Scene = nullptr;
	};
}