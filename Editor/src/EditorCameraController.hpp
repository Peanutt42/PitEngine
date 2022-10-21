#pragma once

#include "Main/CoreInclude.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Pit::Editor {
	class EditorCameraController {
	public:
		// KeyMappings
		static constexpr KeyCode MoveLeft = A;
		static constexpr KeyCode MoveRight = D;
		static constexpr KeyCode MoveForward = W;
		static constexpr KeyCode MoveBackward = S;
		static constexpr KeyCode MoveUp = E;
		static constexpr KeyCode MoveDown = Q;
		static constexpr KeyCode LookLeft = J;
		static constexpr KeyCode LookRight = L;
		static constexpr KeyCode LookUp = I;
		static constexpr KeyCode LookDown = K;
		
		static constexpr float MoveSpeed = 3.f;
		static constexpr float LookSpeed = 2.5f;

		static void MoveInPlaneXZ(glm::vec3& position, glm::vec3& rotation) {
			glm::vec3 rotate{ 0 };
			if (Input::IsKeyDown(LookRight)) rotate.y += 1.f;
			if (Input::IsKeyDown(LookLeft)) rotate.y -= 1.f;
			if (Input::IsKeyDown(LookUp)) rotate.x += 1.f;
			if (Input::IsKeyDown(LookDown)) rotate.x -= 1.f;

			if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
				rotation += LookSpeed * Time::DeltaTime() * glm::normalize(rotate);			

			// limit pitch values between about +/- 85ish degrees
			rotation.x = glm::clamp(rotation.x, -1.5f, 1.5f);
			rotation.y = glm::mod(rotation.y, glm::two_pi<float>());

			float yaw = rotation.y;
			const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
			const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
			const glm::vec3 upDir{ 0.f, -1.f, 0.f };

			glm::vec3 moveDir{ 0.f };
			if (Input::IsKeyDown(MoveForward)) moveDir += forwardDir;
			if (Input::IsKeyDown(MoveBackward)) moveDir -= forwardDir;
			if (Input::IsKeyDown(MoveRight)) moveDir += rightDir;
			if (Input::IsKeyDown(MoveLeft)) moveDir -= rightDir;
			if (Input::IsKeyDown(MoveUp)) moveDir += upDir;
			if (Input::IsKeyDown(MoveDown)) moveDir -= upDir;

			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
				position += MoveSpeed * Time::DeltaTime() * glm::normalize(moveDir);
		}
	};
}