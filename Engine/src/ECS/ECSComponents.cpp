#include "pch.hpp"
#include "ECSComponents.hpp"


#pragma warning (push)
#pragma warning (disable: 4201)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#pragma warning (pop)

namespace Pit::ECS {
	glm::mat4 TransformComponent::GetTransform() {
		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

		return glm::translate(glm::mat4(1.0f), Position)
			* rotation
			* glm::scale(glm::mat4(1.0f), Scale);
	}
}