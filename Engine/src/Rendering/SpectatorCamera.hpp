#pragma once

#include "Core/CoreInclude.hpp"
#include "Camera.hpp"
#include <glm/gtx/rotate_vector.hpp>

namespace Pit {
	class SpectatorCamera {
	public:
		static constexpr float MovementSpeed = 2.5f;
		static constexpr float MouseSensitivity = 0.1f;

		static void Update(Rendering::Camera* cam, KeyCode forwardBinding = KeyCode::W, KeyCode backwardBinding = KeyCode::S, KeyCode leftBinding = KeyCode::A, KeyCode rightBinding = KeyCode::D, KeyCode upBinding = KeyCode::E, KeyCode downBinding = KeyCode::Q, KeyCode fasterBinding = KeyCode::LeftShift, KeyCode slowerBinding = KeyCode::LeftControl) {
			cam->Rotation.x += Input::GetMouseDelta().x * MouseSensitivity;
			cam->Rotation.y += -Input::GetMouseDelta().y * MouseSensitivity;
			cam->Rotation.y = Math::Clamp(cam->Rotation.y, -89.9f, 89.9f);

			glm::vec3 dir{ 0,0,0 };
			if (Input::IsKeyDown(forwardBinding)) dir.z++;
			if (Input::IsKeyDown(backwardBinding)) dir.z--;
			if (Input::IsKeyDown(leftBinding)) dir.x++;
			if (Input::IsKeyDown(rightBinding)) dir.x--;
			if (Input::IsKeyDown(upBinding)) dir.y++;
			if (Input::IsKeyDown(downBinding)) dir.y--;
			if (dir != glm::vec3{ 0,0,0 }) {
				dir = glm::normalize(dir);

				float speed = MovementSpeed * Time::DeltaTime();
				if (Input::IsKeyDown(fasterBinding)) speed *= 2;
				if (Input::IsKeyDown(slowerBinding)) speed /= 2;

				glm::vec3 move{ 0,0,0 };
				glm::vec3 front = glm::normalize(glm::vec3{ cos(glm::radians(cam->Rotation.x)) * cos(glm::radians(cam->Rotation.y)), sin(glm::radians(cam->Rotation.y)), sin(glm::radians(cam->Rotation.x)) * cos(glm::radians(cam->Rotation.y)) });
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