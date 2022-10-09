#pragma once

#include "Main/CoreInclude.hpp"
#include "ECS/ECSComponent.hpp"

namespace Pit::ECS {
	struct EntityComponent : public Component {
		std::string Name;
	};
}