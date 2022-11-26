#pragma once

#include "Core/CoreInclude.hpp"
#pragma warning(push)
#pragma warning(disable: 4996)
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#include <PxPhysicsAPI.h>
#pragma warning(pop)

namespace Pit::Physics {
	struct SceneDescription {
		physx::PxVec3 Gravity = physx::PxVec3(0, -9.81f, 0);
	};

	class Scene {
	public:
		Scene(physx::PxPhysics* physics, physx::PxDefaultCpuDispatcher* dispatcher);
		~Scene();

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