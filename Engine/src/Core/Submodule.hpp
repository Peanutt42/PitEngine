#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit {
	class Submodule {
	public:
		virtual void Init() { }
		virtual void Shutdown() {}
		virtual void Update() {}
	};
}