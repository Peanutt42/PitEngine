#pragma once

#include "Main/CoreInclude.hpp"
#include "ECSInclude.hpp"
#include "ECSWorld.hpp"
#include "Examples/ECSBasicGameloop.hpp"
#include "Main/Submodule.hpp"

namespace Pit {
	DECLARE_EXTERN_PROFILE_STAT_FLOAT(ECSUpdate, ECS, "ECS-Update");

	class ECSSubmodule : public Submodule {
	public:
		virtual void Init() {
			if (!m_ECSWorld.Init())
				PIT_ENGINE_FATAL(Log::ECS, "Error while initializing ECSWorld");

			m_ECSGameloop.Initialize(m_ECSWorld);

			PIT_ENGINE_INFO(Log::ECS, "ECSSubmodule has initialized!");
		}

		virtual void Shutdown() {
			m_ECSGameloop.Shutdown(m_ECSWorld);

			m_ECSWorld.Clear();
			
			PIT_ENGINE_INFO(Log::ECS, "ECSSubmodule has shut down!");
		}

		virtual void Update() {
			SCOPE_STAT(ECSUpdate);

			m_ECSWorld.Update();
		}

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