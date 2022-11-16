#pragma once

#include "Core/CoreInclude.hpp"
#include "ECSWorld.hpp"
#include "ECSComponents.hpp"
#include "ECSEntityHandle.hpp"
#include "Core/Submodule.hpp"

namespace Pit {
	class ECSSubmodule : public Submodule {
	public:
		virtual void Init();

		virtual void Shutdown();

		virtual void Update();

		void ResetECSWorld();

		ECS::World& GetEcsWorld() { return m_ECSWorld; }

	private:
		ECS::WorldSpecs m_ECSWorldSpecs {
			"MainECSWorld"
		};
		ECS::World m_ECSWorld = ECS::World(m_ECSWorldSpecs);
	};
}