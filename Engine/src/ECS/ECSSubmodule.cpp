#include "pch.hpp"
#include "Core/Engine.hpp"
#include "ECSSubmodule.hpp"

using namespace Pit;

void ECSSubmodule::Init() {
	if (!m_ECSWorld.Init())
		PIT_ENGINE_FATAL(Log::ECS, "Error while initializing ECSWorld");

	m_ECSGameloop.Initialize(m_ECSWorld);

	PIT_ENGINE_INFO(Log::ECS, "ECSSubmodule has initialized!");
}

void ECSSubmodule::Shutdown() {
	m_ECSGameloop.Shutdown(m_ECSWorld);

	m_ECSWorld.Clear();

	PIT_ENGINE_INFO(Log::ECS, "ECSSubmodule has shut down!");
}

void ECSSubmodule::Update() {
}