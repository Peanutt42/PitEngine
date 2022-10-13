#pragma once

#include "KeyCode.hpp"
#include <glm/vec2.hpp>

namespace Pit {	
	class Input {
	public:
		static bool IsKeyDown(KeyCode key);
		static KeyState GetKey(KeyCode key);
		static bool IsMouseButtonDown(MouseButton button);

		static glm::vec2 GetMousePosition();

		static void SetCursorMode(CursorMode mode);
	};
}