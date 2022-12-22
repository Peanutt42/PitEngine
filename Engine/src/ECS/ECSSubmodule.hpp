#pragma once

#include "Core/CoreInclude.hpp"
#include "ECSWorld.hpp"
#include "ECSComponents.hpp"
#include "ECSEntityHandle.hpp"
#include "Core/Submodule.hpp"

namespace Pit {
	/// <summary>
	/// Submodule that handles the ECSWorld.
	/// TODO: Add Loading/Unloading/Switching Worlds
	/// TODO: Add support for multiple Worlds at the same time
	/// </summary>
	class ECSSubmodule : public Submodule {
	public:
		virtual void Init();

		virtual void Shutdown();

		virtual void Update();

		void ResetECSWorld();

		ECS::World& GetEcsWorld();

	private:
		ECS::WorldSpecs m_ECSWorldSpecs {
			"MainECSWorld"
		};
		ECS::World m_ECSWorld = ECS::World(m_ECSWorldSpecs);
	};
}