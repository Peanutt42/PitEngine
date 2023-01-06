#pragma once

#include "Core/CoreInclude.hpp"
#include "ECSScene.hpp"

namespace Pit::ECS {
	class SceneSerializer {
	public:
		static const bool Serialize(const std::filesystem::path& filepath, Scene& scene);
		static const bool Deserialize(const std::filesystem::path& filepath, Scene& scene);
	};
}