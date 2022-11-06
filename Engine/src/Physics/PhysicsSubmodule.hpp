#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"
#include "PhysicScene.hpp"
#include <PxPhysicsAPI.h>


namespace Pit {
	class PhysicsSubmodule : public Submodule {
	public:
		virtual void Init();

		virtual void Shutdown();

		virtual void Update();

	private:
		float m_FixedTimestep = 1.f / 60.f;

		// Physx Stuff
		physx::PxDefaultAllocator m_DefaultAllocatorCallback;
		physx::PxDefaultErrorCallback m_DefaultErrorCallback;
		physx::PxDefaultCpuDispatcher* m_Dispatcher = nullptr;
		physx::PxTolerancesScale m_ToleranceScale;
		physx::PxFoundation* m_Foundation = nullptr;
		physx::PxPhysics* m_Physics = nullptr;

		// Physx Scene
		Physics::Scene* m_Scene = nullptr;
		physx::PxMaterial* m_Material = nullptr;
		physx::PxRigidDynamic* body;

		// Physx Debugger
		physx::PxPvd* m_Pvd = nullptr;

		void _InitPhysx();
		void _CreateScene();
	};
}