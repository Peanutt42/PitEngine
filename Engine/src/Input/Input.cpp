#include "pch.hpp"
#include "Core/Engine.hpp"
#include "Input.hpp"
#include "Core/Engine.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include <GLFW/glfw3.h>
#include <conio.h>

namespace Pit {

	glm::vec2 Input::m_LastMousePos = { 2560 / 2, 1440 / 2 };
	glm::vec2 Input::m_LastMousePosDelta;
	KeyState Input::s_KeyStates[KEY_COUNT];
	bool Input::s_KeyStateChanged[KEY_COUNT];
	bool Input::s_MouseButtonStates[MOUSEBUTTON_COUNT];
	bool Input::s_MosueButtonStateChanged[MOUSEBUTTON_COUNT];

	static GLFWwindow* GetWindow() {
		return Engine::Rendering()->Window->GetWindowHandle();
	}

	void Input::Update() {
		if (Engine::GetInfo().Headless) return; // TODO: Read console input as commands, not that straight forward though

		GLFWwindow* window = GetWindow();
		for (int i = MIN_KEY; i < KEY_COUNT; i++) {
			KeyState state = Cast<KeyState>(glfwGetKey(window, i));
			s_KeyStateChanged[i] = s_KeyStates[i] != state;
			s_KeyStates[i] = state;
		}
		for (int i = 0; i < MOUSEBUTTON_COUNT; i++) {
			bool state = glfwGetMouseButton(window, i) == GLFW_PRESS;
			s_MosueButtonStateChanged[i] = s_MouseButtonStates[i] != state;
			s_MouseButtonStates[i] = state;
		}

		double mouse_x, mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		glm::vec2 currentMousePos = { Cast<float>((int)mouse_x), Cast<float>((int)mouse_y) };
		m_LastMousePosDelta = currentMousePos - m_LastMousePos;
		m_LastMousePos = currentMousePos;
	}

	bool Input::IsKeyUp(KeyCode key) {
		return s_KeyStates[key] == KeyState::Released;
	}

	bool Input::IsKeyDown(KeyCode key) {
		return s_KeyStates[key] == KeyState::Pressed || s_KeyStates[key] == KeyState::Held;
	}

	bool Input::IsKeyPressed(KeyCode key) {
		return s_KeyStateChanged[key] && s_KeyStates[key] == KeyState::Pressed || s_KeyStates[key] == KeyState::Held;
	}

	bool Input::IsKeyReleased(KeyCode key) {
		return s_KeyStateChanged[key] && s_KeyStates[key] == KeyState::Released;
	}

	KeyState Input::GetKey(KeyCode key) {
		return s_KeyStates[key];
	}

	bool Input::IsMouseButtonUp(MouseButton button) {
		return !s_MouseButtonStates[button];
	}

	bool Input::IsMouseButtonDown(MouseButton button) {
		return s_MouseButtonStates[button];
	}

	bool Input::IsMouseButtonPressed(MouseButton button) {
		return s_MosueButtonStateChanged[button] && s_MouseButtonStates[button];
	}

	bool Input::IsMouseButtonReleased(MouseButton button) {
		return s_MosueButtonStateChanged[button] && !s_MouseButtonStates[button];
	}

	void Input::SetMousePos(const glm::vec2& newPos) {
		glfwSetCursorPos(GetWindow(), newPos.x, newPos.y);
		m_LastMousePos = newPos;
	}

	void Input::SetCursorMode(CursorMode mode) {
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
	}
}