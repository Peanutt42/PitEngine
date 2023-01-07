#pragma once

#include "Core/CoreInclude.hpp"
#include "ECSScene.hpp"
#include "ECSComponents.hpp"
#include "ECSEntityHandle.hpp"
#include "Core/Submodule.hpp"

namespace Pit {
	/// <summary>
	/// Submodule that handles the ECSWorld.
	/// </summary>
	class ECSSubmodule : public Submodule {
	public:
		ECSSubmodule();

		virtual void Init();

		virtual void Shutdown();

		virtual void Update();

		void ResetECSWorld();

		ECS::Scene& GetEcsWorld();

	private:
		ECS::Scene m_ECSScene;
	};
}