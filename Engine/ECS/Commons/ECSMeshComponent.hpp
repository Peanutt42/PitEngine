#pragma once

#include "Rendering/RenderingInclude.hpp"
#include "Rendering/Vulkan/VulkanModel.hpp"
#include <memory>

namespace Pit::ECS {
	struct MeshComponent {
		std::shared_ptr<Rendering::Vulkan::Model> model;
		glm::vec3 color{};
	};
}