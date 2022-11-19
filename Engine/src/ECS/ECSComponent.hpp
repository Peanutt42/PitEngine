#pragma once

namespace Pit::ECS {
	/// <summary>
	/// Base struct for every ECS Component
	/// (Holds if component is active(enabled))
	/// </summary>
	struct Component {
		bool active = true;
	};
}