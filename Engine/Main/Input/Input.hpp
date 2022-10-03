#pragma once

#include "KeyCode.hpp"
#include <glm/glm.hpp>

namespace Pit {	
	class Input {
	public:
		static void SetWindow(GLFWwindow* window);

		static bool IsKeyDown(KeyCode keycode);
		static bool IsMouseButtonDown(MouseButton button);

		static glm::vec2 GetMousePosition();

		static void SetCursorMode(CursorMode mode);

	private:
		static GLFWwindow* m_Window;
	};
}