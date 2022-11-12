#include "pch.hpp"
#include "Core/Engine.hpp"
#include "ECSSubmodule.hpp"

using namespace Pit;

void ECSSubmodule::Init() {
	PIT_PROFILE_FUNCTION();

	if (!m_ECSWorld.Init())
		PIT_ENGINE_FATAL(ECS, "Error while initializing ECSWorld");

	m_ECSGameloop.Initialize(m_ECSWorld);
}

void ECSSubmodule::Shutdown() {
	PIT_PROFILE_FUNCTION();

	m_ECSGameloop.Shutdown(m_ECSWorld);

	m_ECSWorld.Clear();
}

void ECSSubmodule::Update() {
	PIT_PROFILE_FUNCTION();

	m_ECSGameloop.Update(m_ECSWorld);
}