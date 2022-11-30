#pragma once

#include "Core/CoreInclude.hpp"
#include "Camera.hpp"
#include <glm/gtx/rotate_vector.hpp>

namespace Pit {
	class SpectatorCamera {
	public:
		static void Update(Rendering::Camera* cam, const KeyCode& forwardBinding = KeyCode::W, const KeyCode& backwardBinding = KeyCode::S, const KeyCode& leftBinding = KeyCode::A, const KeyCode& rightBinding = KeyCode::D, const KeyCode& upBinding = KeyCode::E, const KeyCode& downBinding = KeyCode::Q, const KeyCode& fasterBinding = KeyCode::LeftShift, const KeyCode& slowerBinding = KeyCode::LeftControl) {
			cam->ProcessMouseMovement(Input::GetMouseDelta().x, -Input::GetMouseDelta().y);

			glm::vec3 dir{ 0,0,0 };
			if (Input::IsKeyDown(forwardBinding)) dir.z++;
			if (Input::IsKeyDown(backwardBinding)) dir.z--;
			if (Input::IsKeyDown(leftBinding)) dir.x++;
			if (Input::IsKeyDown(rightBinding)) dir.x--;
			if (Input::IsKeyDown(upBinding)) dir.y++;
			if (Input::IsKeyDown(downBinding)) dir.y--;
			if (dir != glm::vec3{ 0,0,0 }) {
				dir = glm::normalize(dir);

				float speed = cam->MovementSpeed * Time::DeltaTime();
				if (Input::IsKeyDown(fasterBinding)) speed *= 2;
				if (Input::IsKeyDown(slowerBinding)) speed /= 2;

				glm::vec3 move{ 0,0,0 };
				move += dir.z * cam->Front;
				move += dir.x * -cam->Right;
				move += dir.y * cam->Up;
				cam->Position += glm::normalize(move) * speed;
			}
		}
	};
}