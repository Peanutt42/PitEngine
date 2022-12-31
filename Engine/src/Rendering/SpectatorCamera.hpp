#pragma once

#include "Core/CoreInclude.hpp"
#include "Camera.hpp"
#include <glm/gtx/rotate_vector.hpp>

namespace Pit {
	class SpectatorCamera {
	public:
		static constexpr float MovementSpeed = 2.5f;
		static constexpr float MouseSensitivity = 0.1f;

		static void Update(Rendering::Camera* cam, const glm::vec3& moveDir, const glm::vec2& lookDir, bool moveFaster, bool moveSlower) {
			if (lookDir != glm::vec2{0, 0}) {
				cam->Rotation.x += lookDir.x * MouseSensitivity;
				cam->Rotation.y += lookDir.y * MouseSensitivity;
			}
			cam->Rotation.y = Math::Clamp(cam->Rotation.y, -89.99f, 89.99f);
			if (moveDir != glm::vec3{ 0,0,0 }) {
				glm::vec3 dir;
				if (glm::length(moveDir) > 1.0f)
					dir = glm::normalize(moveDir);
				else dir = moveDir;

				float speed = MovementSpeed * glm::length(dir) * Time::DeltaTime();
				if (moveFaster) speed *= 2;
				if (moveSlower) speed /= 2;

				glm::vec3 move{ 0,0,0 };
				glm::vec3 front = glm::normalize(glm::vec3{ cos(glm::radians(cam->Rotation.x)) * cos(glm::radians(cam->Rotation.y)),
															sin(glm::radians(cam->Rotation.y)),
															sin(glm::radians(cam->Rotation.x)) * cos(glm::radians(cam->Rotation.y)) });
				glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
				glm::vec3 up = glm::normalize(glm::cross(right, front));
				move += dir.z * front;
				move += dir.x * -right;
				move += dir.y * up;
				cam->Position += glm::normalize(move) * speed;
			}
		}
	};
}