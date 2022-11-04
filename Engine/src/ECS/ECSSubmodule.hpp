#pragma once

#include "Core/CoreInclude.hpp"
#include "ECSWorld.hpp"
#include "Examples/ECSBasicGameloop.hpp"
#include "ECSComponents.hpp"
#include "ECSEntityHandle.hpp"
#include "Core/Submodule.hpp"

namespace Pit {
	class ECSSubmodule : public Submodule {
	public:
		virtual void Init();

		virtual void Shutdown();

		virtual void Update();

		void ResetECSWorld() {
			m_ECSGameloop.Shutdown(m_ECSWorld);

			m_ECSWorld.Clear();
			m_ECSWorld = ECS::World(m_ECSWorldSpecs);
			if (!m_ECSWorld.Init())
				PIT_ENGINE_FATAL(Log::ECS, "Error while initializing ECSWorld");

			m_ECSGameloop.Initialize(m_ECSWorld);
		}

		ECS::World& GetEcsWorld() { return m_ECSWorld; }

	private:
		ECS::WorldSpecs m_ECSWorldSpecs {
			"MainECSWorld"
		};
		ECS::World m_ECSWorld = ECS::World(m_ECSWorldSpecs);
		ECS::Example::BasicGameloop m_ECSGameloop;
	};
}