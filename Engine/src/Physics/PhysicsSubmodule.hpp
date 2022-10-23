#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"

namespace Pit {
	class PhysicsSubmodule : public Submodule {
	public:
		virtual void Init();

		virtual void Shutdown();

		virtual void Update();
	};
}