#include "ECSSubsystem.hpp"


using namespace Pit::ECS;

ECSSubsystem* ECSSubsystem::m_Instance = nullptr;


ECSSubsystem::ECSSubsystem() {
	m_Instance = this;

	ECSWorld = new World(ECSWorldSpecs);
	if (!ECSWorld->Init())
		PIT_ENGINE_ERR("Error while initializing ECSWorld");

	ECSGameloop = new Example::BasicGameloop();
	ECSGameloop->Initialize(*ECSWorld);

	PIT_ENGINE_INFO("ECSSubsystem has started!");
}

ECSSubsystem::~ECSSubsystem() {
	ECSGameloop->Shutdown(*ECSWorld);

	if (ECSWorld) {
		ECSWorld->Clear();
		delete ECSWorld;
	}

	m_Instance = nullptr;

	PIT_ENGINE_INFO("ECSSubsystem has shut down!");
}

void ECSSubsystem::Tick(float DeltaTime) {
	ECSWorld->Update(DeltaTime);
}

void ECSSubsystem::ResetECSWorld() {
	if (ECSWorld) {
		ECSWorld->Clear();
		delete ECSWorld;
	}

	ECSWorld = new World(ECSWorldSpecs);
	if (!ECSWorld->Init())
		PIT_ENGINE_ERR("Error while initializing ECSWorld");
}