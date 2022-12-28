#include "pch.hpp"
#include "Core/Engine.hpp"
#include "Input.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include <GLFW/glfw3.h>
#include <conio.h>

namespace Pit {

	glm::vec2 Input::m_LastMousePos = { 2560 / 2, 1440 / 2 };
	glm::vec2 Input::m_LastMousePosDelta;
	ButtonState Input::s_KeyStates[KEY_COUNT];
	bool Input::s_KeyStateChanged[KEY_COUNT];
	bool Input::s_MouseButtonStates[MOUSEBUTTON_COUNT];
	bool Input::s_MosueButtonStateChanged[MOUSEBUTTON_COUNT];
	ButtonState Input::s_ControllerButtonStates[MAX_CONTROLLER_COUNT][CONTROLLER_BUTTON_COUNT];
	bool Input::s_ControllerButtonStateChanged[MAX_CONTROLLER_COUNT][CONTROLLER_BUTTON_COUNT];
	float Input::s_ControllerAxises[MAX_CONTROLLER_COUNT][CONTROLLER_AXIS_COUNT];
	ControllerType Input::s_CurrentControllerType[MAX_CONTROLLER_COUNT];

	Keybindings Input::s_Bindings;
	bool Input::s_UseKeyboard = false;

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

		for (int controller = 0; controller < MAX_CONTROLLER_COUNT; controller++) {
			if (glfwJoystickPresent(controller)) {
				int numAxes;
				const float* axises = glfwGetJoystickAxes(controller, &numAxes);
				for (int i = 0; i < numAxes; i++) s_ControllerAxises[controller][i] = axises[i];

				int numButtons;
				const unsigned char* buttons = glfwGetJoystickButtons(controller, &numButtons);
				for (int i = 0; i < numButtons; i++) {
					ButtonState state = Cast<ButtonState>(buttons[i]);
					s_ControllerButtonStateChanged[controller][i] = s_ControllerButtonStates[controller][i] != state;
					s_ControllerButtonStates[controller][i] = state;
				}
				s_CurrentControllerType[controller] = GetControllerType(glfwGetJoystickName(controller));
				[[maybe_unused]] bool pressed = buttons[GetControllerButton(s_CurrentControllerType[controller], ControllerButton::Down)] == GLFW_PRESS;
			}
		}

		GLFWwindow* window = GetWindow();
		for (int i = MIN_KEY; i < KEY_COUNT; i++) {
			ButtonState state = Cast<ButtonState>(glfwGetKey(window, i));
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

		if (IsControllerButtonPressed(ControllerId::ID1, ControllerButton::Down)) std::cout << "Pressed\n";
		if (IsControllerButtonReleased(ControllerId::ID1, ControllerButton::Down)) std::cout << "Released\n";
	}

	const bool Input::IsKeyUp(KeyCode key) {
		return s_KeyStates[Cast<uint16_t>(key)] == ButtonState::Released;
	}

	const bool Input::IsKeyDown(KeyCode key) {
		return s_KeyStates[Cast<uint16_t>(key)] == ButtonState::Pressed || s_KeyStates[Cast<uint16_t>(key)] == ButtonState::Held;
	}

	const bool Input::IsKeyPressed(KeyCode key) {
		return s_KeyStateChanged[Cast<uint16_t>(key)] && s_KeyStates[Cast<uint16_t>(key)] == ButtonState::Pressed || s_KeyStates[Cast<uint16_t>(key)] == ButtonState::Held;
	}

	const bool Input::IsKeyReleased(KeyCode key) {
		return s_KeyStateChanged[Cast<uint16_t>(key)] && s_KeyStates[Cast<uint16_t>(key)] == ButtonState::Released;
	}

	const ButtonState Input::GetKey(KeyCode key) {
		return s_KeyStates[Cast<uint16_t>(key)];
	}

	const bool Input::IsMouseButtonUp(MouseButton button) {
		return !s_MouseButtonStates[Cast<uint16_t>(button)];
	}

	const bool Input::IsMouseButtonDown(MouseButton button) {
		return s_MouseButtonStates[Cast<uint16_t>(button)];
	}

	const bool Input::IsMouseButtonPressed(MouseButton button) {
		return s_MosueButtonStateChanged[Cast<uint16_t>(button)] && s_MouseButtonStates[Cast<uint16_t>(button)];
	}

	const bool Input::IsMouseButtonReleased(MouseButton button) {
		return s_MosueButtonStateChanged[Cast<uint16_t>(button)] && !s_MouseButtonStates[Cast<uint16_t>(button)];
	}

	const bool Input::IsControllerConnected(ControllerId controllerId) {
		int controller = Cast<int>(controllerId);
		return glfwJoystickPresent(Cast<int>(controller));
	}

	const glm::vec2 Input::GetControllerAxis(ControllerId controllerId, ControllerAxis axis) {
		int controller = Cast<int>(controllerId);
		switch (axis) {
		default:
		case ControllerAxis::None: return glm::vec2{ 0, 0 };
		case ControllerAxis::Left: return glm::vec2{ s_ControllerAxises[controller][CONTROLLER_LEFT_AXIS_X],
														  s_ControllerAxises[controller][CONTROLLER_LEFT_AXIS_Y] };
		case ControllerAxis::Right: return glm::vec2{ s_ControllerAxises[controller][CONTROLLER_RIGHT_AXIS_X],
														  s_ControllerAxises[controller][CONTROLLER_RIGHT_AXIS_Y] };
		}
	}

	const float Input::GetControllerPedal(ControllerId controllerId, ControllerPedal pedal) {
		int controller = Cast<int>(controllerId);
		switch (pedal) {
		default:
		case ControllerPedal::None: return 0.f;
		case ControllerPedal::Left: return s_ControllerAxises[controller][CONTROLLER_LEFT_TRIGGER];
		case ControllerPedal::Right: return s_ControllerAxises[controller][CONTROLLER_RIGHT_TRIGGER];
		}
	}

	const bool Input::IsControllerButtonUp(ControllerId controllerId, ControllerButton button) {
		int controller = Cast<int>(controllerId);
		return s_ControllerButtonStates[controller][GetControllerButton(s_CurrentControllerType[controller], button)] == ButtonState::Pressed;
	}
	const bool Input::IsControllerButtonDown(ControllerId controllerId, ControllerButton button) {
		int controller = Cast<int>(controllerId);
		return s_ControllerButtonStates[controller][GetControllerButton(s_CurrentControllerType[controller], button)] == ButtonState::Released;
	}
	const bool Input::IsControllerButtonPressed(ControllerId controllerId, ControllerButton button) {
		int controller = Cast<int>(controllerId);
		return s_ControllerButtonStates[controller][GetControllerButton(s_CurrentControllerType[controller], button)] == ButtonState::Pressed &&
														    s_ControllerButtonStateChanged[controller][GetControllerButton(s_CurrentControllerType[controller], button)];
	}
	const bool Input::IsControllerButtonReleased(ControllerId controllerId, ControllerButton button) {
		int controller = Cast<int>(controllerId);
		return s_ControllerButtonStates[controller][GetControllerButton(s_CurrentControllerType[controller], button)] == ButtonState::Released &&
															s_ControllerButtonStateChanged[controller][GetControllerButton(s_CurrentControllerType[controller], button)];
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

	void Input::AddBinding(const String& name, ControllerButton controllerButton) { s_Bindings.AddBinding(name, controllerButton); }
	
	void Input::SetBinding(const String& name, KeyCode newKeycode) { s_Bindings.SetBinding(name, newKeycode); }
	
	void Input::SetBinding(const String& name, MouseButton newMouseButton) { s_Bindings.SetBinding(name, newMouseButton); }

	void Input::SetBinding(const String& name, ControllerButton newControllerButton) { s_Bindings.SetBinding(name, newControllerButton); }
	
	const KeyCode Input::GetBinding(const String& name) { return s_Bindings.GetBinding(name); }
	
	const MouseButton Input::GetMouseBinding(const String& name) { return s_Bindings.GetMouseBinding(name); }

	const ControllerButton Input::GetControllerButtonBinding(const String& name) { return s_Bindings.GetControllerButtonBinding(name); }

	const bool Input::IsBindingUp(const String& name) {
		if (s_UseKeyboard) {
			KeyCode key = GetBinding(name);
			MouseButton mousebutton = GetMouseBinding(name);
			if (key != KeyCode::None) return IsKeyUp(key);
			if (mousebutton != MouseButton::None) return IsMouseButtonUp(mousebutton);
			return true;
		}
		else {
			ControllerButton button = GetControllerButtonBinding(name);
			if (button != ControllerButton::None)
				return IsControllerButtonUp(ControllerId::ID1, button); // TODO: Allow multiple controllers
			else
				return true;
		}
		return true;
	}

	const bool Input::IsBindingDown(const String& name) {
		if (s_UseKeyboard) {
			KeyCode key = GetBinding(name);
			MouseButton mousebutton = GetMouseBinding(name);
			if (key != KeyCode::None) return IsKeyDown(key);
			if (mousebutton != MouseButton::None) return IsMouseButtonDown(mousebutton);
			return false;
		}
		else {
			ControllerButton button = GetControllerButtonBinding(name);
			if (button != ControllerButton::None)
				return IsControllerButtonDown(ControllerId::ID1, button); // TODO: Allow multiple controllers
			else
				return false;
		}
		return false;
	}

	const bool Input::IsBindingPressed(const String& name) {
		if (s_UseKeyboard) {
			KeyCode key = GetBinding(name);
			MouseButton mousebutton = GetMouseBinding(name);
			if (key != KeyCode::None) return IsKeyPressed(key);
			if (mousebutton != MouseButton::None) return IsMouseButtonPressed(mousebutton);
			return false;
		}
		else {
			ControllerButton button = GetControllerButtonBinding(name);
			if (button != ControllerButton::None)
				return IsControllerButtonPressed(ControllerId::ID1, button); // TODO: Allow multiple controllers
			else
				return false;
		}
		return false;
	}

	const bool Input::IsBindingReleased(const String& name) {
		if (s_UseKeyboard) {
			KeyCode key = GetBinding(name);
			MouseButton mousebutton = GetMouseBinding(name);
			if (key != KeyCode::None) return IsKeyReleased(key);
			if (mousebutton != MouseButton::None) return IsMouseButtonReleased(mousebutton);
			return false;
		}
		else {
			ControllerButton button = GetControllerButtonBinding(name);
			if (button != ControllerButton::None)
				return IsControllerButtonReleased(ControllerId::ID1, button); // TODO: Allow multiple controllers
			else
				return false;
		}
		return false;
	}
	
	bool Input::RemoveBinding(const String& name) { return s_Bindings.RemoveBinding(name); }
}