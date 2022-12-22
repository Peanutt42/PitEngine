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

	Keybindings Input::s_Bindings;

	static GLFWwindow* GetWindow() {
		return Engine::Rendering()->Window->GetWindowHandle();
	}

	void Input::Init() {
		if (Engine::GetSettings().Headless) return; // TODO: Read console input as commands, not that straight forward though

		s_Bindings.Deserize(FileSystem::GetConfigDir() + "Keybindings.ini");

		Engine::SaveConfigEvent += []() {Input::s_Bindings.Serialize(FileSystem::GetConfigDir() + "Keybindings.ini"); };
	}

	void Input::Update() {
		if (Engine::GetSettings().Headless) return; // TODO: Read console input as commands, not that straight forward though

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
		return s_KeyStates[Cast<uint16_t>(key)] == KeyState::Released;
	}

	bool Input::IsKeyDown(KeyCode key) {
		return s_KeyStates[Cast<uint16_t>(key)] == KeyState::Pressed || s_KeyStates[Cast<uint16_t>(key)] == KeyState::Held;
	}

	bool Input::IsKeyPressed(KeyCode key) {
		return s_KeyStateChanged[Cast<uint16_t>(key)] && s_KeyStates[Cast<uint16_t>(key)] == KeyState::Pressed || s_KeyStates[Cast<uint16_t>(key)] == KeyState::Held;
	}

	bool Input::IsKeyReleased(KeyCode key) {
		return s_KeyStateChanged[Cast<uint16_t>(key)] && s_KeyStates[Cast<uint16_t>(key)] == KeyState::Released;
	}

	KeyState Input::GetKey(KeyCode key) {
		return s_KeyStates[Cast<uint16_t>(key)];
	}

	bool Input::IsMouseButtonUp(MouseButton button) {
		return !s_MouseButtonStates[Cast<uint16_t>(button)];
	}

	bool Input::IsMouseButtonDown(MouseButton button) {
		return s_MouseButtonStates[Cast<uint16_t>(button)];
	}

	bool Input::IsMouseButtonPressed(MouseButton button) {
		return s_MosueButtonStateChanged[Cast<uint16_t>(button)] && s_MouseButtonStates[Cast<uint16_t>(button)];
	}

	bool Input::IsMouseButtonReleased(MouseButton button) {
		return s_MosueButtonStateChanged[Cast<uint16_t>(button)] && !s_MouseButtonStates[Cast<uint16_t>(button)];
	}

	void Input::SetMousePos(const glm::vec2& newPos) {
		glfwSetCursorPos(GetWindow(), newPos.x, newPos.y);
		m_LastMousePos = newPos;
	}

	void Input::SetCursorMode(CursorMode mode) {
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
	}
	
	void Input::AddBinding(const String& name, KeyCode keycode) { s_Bindings.AddBinding(name, keycode); }
	
	void Input::AddBinding(const String& name, MouseButton mouseButton) { s_Bindings.AddBinding(name, mouseButton); }
	
	void Input::SetBinding(const String& name, KeyCode newKeycode) { s_Bindings.SetBinding(name, newKeycode); }
	
	void Input::SetBinding(const String& name, MouseButton newMouseButton) { s_Bindings.SetBinding(name, newMouseButton); }
	
	KeyCode Input::GetBinding(const String& name) { return s_Bindings.GetBinding(name); }
	
	MouseButton Input::GetMouseBinding(const String& name) { return s_Bindings.GetMouseBinding(name); }
	
	bool Input::RemoveBinding(const String& name) { return s_Bindings.RemoveBinding(name); }
}