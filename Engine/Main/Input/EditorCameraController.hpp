#pragma once

#include "Main/CoreInclude.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Pit {
	class EditorCameraController {
	public:
		static void MoveCam(glm::vec3& pos, glm::vec3& rot) {
			glm::vec3 rotate{ 0 };
			if (Input::IsKeyDown(KeyCode::Right)) rotate.y += 1.f;
			if (Input::IsKeyDown(KeyCode::Left)) rotate.y -= 1.f;
			if (Input::IsKeyDown(KeyCode::Up)) rotate.x += 1.f;
			if (Input::IsKeyDown(KeyCode::Down)) rotate.x -= 1.f;

			if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
				rot += 1.5f * Time::DeltaTime * glm::normalize(rotate);

			rot.y = glm::mod(rot.y, glm::two_pi<float>());

			float yaw = rot.y;
			const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
			const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
			const glm::vec3 upDir{ 0.f, -1.f, 0.f };

			glm::vec3 moveDir{ 0.f };
			if (Input::IsKeyDown(KeyCode::W)) moveDir += forwardDir;
			if (Input::IsKeyDown(KeyCode::S)) moveDir -= forwardDir;
			if (Input::IsKeyDown(KeyCode::D)) moveDir += rightDir;
			if (Input::IsKeyDown(KeyCode::A)) moveDir -= rightDir;
			if (Input::IsKeyDown(KeyCode::E)) moveDir += upDir;
			if (Input::IsKeyDown(KeyCode::Q)) moveDir -= upDir;

			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
				pos += 3 * Time::DeltaTime * glm::normalize(moveDir);
			}
		}
	};
}