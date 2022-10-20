#pragma once

#include "KeyCode.hpp"
#include <glm/vec2.hpp>

namespace Pit {	
	class Input {
	public:
		static void Update();

		static bool IsKeyDown(KeyCode key);
		static KeyState GetKey(KeyCode key);
		static bool IsMouseButtonDown(MouseButton button);

		static const glm::vec2& GetMousePos() { return m_LastMousePos; }
		static const glm::vec2& GetMouseDelta() { return m_LastMousePosDelta; }

		static glm::vec2 GetMousePositionDelta();

		static void SetCursorMode(CursorMode mode);

	private:
		static glm::vec2 _GetMousePosition();
		static glm::vec2 m_LastMousePos;
		static glm::vec2 m_LastMousePosDelta;
	};
}