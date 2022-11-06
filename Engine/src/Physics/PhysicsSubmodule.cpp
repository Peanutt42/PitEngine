#include "pch.hpp"
#include "PhysicsSubmodule.hpp"

using namespace Pit;

void PhysicsSubmodule::Init() {
	PIT_PROFILE_FUNCTION();

	_InitPhysx();
	_CreateScene();
}

void PhysicsSubmodule::Shutdown() {
	PIT_PROFILE_FUNCTION();

	delete m_Scene;
}

void PhysicsSubmodule::Update() {
	PIT_PROFILE_FUNCTION();

	using namespace std::chrono;
	using clock = high_resolution_clock;
	using timepoint = time_point<clock>;
	static timepoint currentTime = clock::now();
	static float accumulator = 0;

	timepoint newTime = clock::now();
	auto frameTime = duration_cast<nanoseconds>(newTime - currentTime).count() * 0.000000001f;
	currentTime = newTime;

	accumulator += frameTime;
	accumulator = Math::Clamp(accumulator, 0.f, .25f);

	while (accumulator >= m_FixedTimestep) {
		PIT_PROFILE_FUNCTION("Pit::PhysicsSubmodule::Update::Step");

		m_Scene->Simulate(m_FixedTimestep);
		auto transform = body->getGlobalPose();
		std::cout << transform.p.x << ", " << transform.p.y << ", " << transform.p.z << std::endl;

		accumulator -= m_FixedTimestep;
	}
}

void PhysicsSubmodule::_InitPhysx() {
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocatorCallback, m_DefaultErrorCallback);
	if (!m_Foundation) PIT_ENGINE_FATAL(Log::Phyisics, "PxCreateFoundation failed!");
	m_Pvd = PxCreatePvd(*m_Foundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	//mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(),true, mPvd);
	m_ToleranceScale.length = 100;        // typical length of an object
	m_ToleranceScale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice
	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale, true, m_Pvd);
	//mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale);

	m_Dispatcher = physx::PxDefaultCpuDispatcherCreate(2);

	m_Scene = new Physics::Scene(m_Physics, m_Dispatcher);

	physx::PxPvdSceneClient* pvdClient = m_Scene->GetNative()->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

}

void PhysicsSubmodule::_CreateScene() {
	m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*m_Physics, physx::PxPlane(0, 1, 0, 50), *m_Material);
	m_Scene->AddActor(*groundPlane);

	float halfExtent = .5f;
	physx::PxShape* shape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);
	physx::PxU32 size = 10;
	physx::PxTransform t(physx::PxVec3(0));
	for (physx::PxU32 i = 0; i < size; i++) {
		for (physx::PxU32 j = 0; j < size - i; j++) {
			physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
			body = m_Physics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			m_Scene->AddActor(*body);
		}
	}
	shape->release();
}
