#pragma once

#include "Core/CoreInclude.hpp"
#include "Core\Submodule.hpp"

namespace Pit {
	class ScriptingSubmodule : public Submodule {
	public:
		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void Update() override;

	private:
		void InitMono(), ShutdownMono();
	};
}