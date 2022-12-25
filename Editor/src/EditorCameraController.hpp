#pragma once

#include "Core/CoreInclude.hpp"
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
		static constexpr float LookSpeed = .001f;

		static void MoveInPlaneXZ(glm::vec3& position, glm::vec3& rotation) {
			glm::vec3 rotate;
			if (Input::IsMouseButtonDown(MouseButton::Right)) {
				Input::SetCursorMode(CursorMode::Locked);
				rotate = glm::vec3{ -Input::GetMouseDelta().y, Input::GetMouseDelta().x, 0 };
			}
			else
				Input::SetCursorMode(CursorMode::Normal);

			if (glm::dot(rotate, rotate) > EPSILON)
				rotation += LookSpeed * rotate;

			// limit pitch values between about +/- 85ish degrees
			rotation.x = glm::clamp(rotation.x, -1.5f, 1.5f);
			rotation.y = glm::mod(rotation.y, glm::two_pi<float>());

			float yaw = rotation.y;
			const glm::vec3 forwardDir{ sin(yaw), 0, cos(yaw) };
			const glm::vec3 rightDir{ forwardDir.z, 0, -forwardDir.x };
			const glm::vec3 upDir{ 0, -1, 0 };

			glm::vec3 moveDir{ 0 };
			if (Input::IsKeyDown(MoveForward)) moveDir += forwardDir;
			if (Input::IsKeyDown(MoveBackward)) moveDir -= forwardDir;
			if (Input::IsKeyDown(MoveRight)) moveDir += rightDir;
			if (Input::IsKeyDown(MoveLeft)) moveDir -= rightDir;
			if (Input::IsKeyDown(MoveUp)) moveDir += upDir;
			if (Input::IsKeyDown(MoveDown)) moveDir -= upDir;

			float speed = MoveSpeed;
			if (Input::IsKeyDown(KeyCode::LeftShift)) speed *= 2;
			if (Input::IsKeyDown(KeyCode::LeftControl)) speed /= 2;

			if (glm::dot(moveDir, moveDir) > EPSILON)
				position += speed * Time::DeltaTime() * glm::normalize(moveDir);
		}
	};
}