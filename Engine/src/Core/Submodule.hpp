#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit {
	/// <summary>
	/// Base class for every major topic of a GameEngine
	/// like Audio, Input, Graphics, AssetManagment, Physics, Networking, Serialization...
	/// </summary>
	class Submodule {
	public:
		virtual void Init() { }
		virtual void Shutdown() {}
		virtual void Update() {}
	};
}