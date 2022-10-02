#pragma once

#include "Rendering/RenderingInclude.hpp"

namespace Pit::ECS {
	struct Transform2DComponent {
		glm::vec2 position{};
		glm::vec2 scale{1.f, 1.f};
		float rotation;

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, 0.f}, {0.f, scale.y} };
			return rotMatrix * scaleMat;
		}
	};
}