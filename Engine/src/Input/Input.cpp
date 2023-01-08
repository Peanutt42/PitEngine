#include "pch.hpp"
#include "Core/Engine.hpp"
#include "Input.hpp"
#include "Rendering/Window.hpp"
#include <GLFW/glfw3.h>
#include <conio.h>

namespace Pit {

	glm::vec2 Input::m_LastMousePos = { 2560 / 2, 1440 / 2 };
	glm::vec2 Input::m_LastMousePosDelta;
	ButtonState Input::s_KeyStates[KEY_COUNT];
	bool Input::s_KeyStateChanged[KEY_COUNT];
	bool Input::s_MouseButtonStates[MOUSEBUTTON_COUNT];
	bool Input::s_MosueButtonStateChanged[MOUSEBUTTON_COUNT];
	bool Input::s_ControllerActiveStates[MAX_CONTROLLER_COUNT];
	ButtonState Input::s_ControllerButtonStates[MAX_CONTROLLER_COUNT][CONTROLLER_BUTTON_COUNT];
	bool Input::s_ControllerButtonStateChanged[MAX_CONTROLLER_COUNT][CONTROLLER_BUTTON_COUNT];
	float Input::s_ControllerAxises[MAX_CONTROLLER_COUNT][CONTROLLER_AXIS_COUNT];
	ControllerType Input::s_ControllerTypes[MAX_CONTROLLER_COUNT];

	InputBinding Input::s_InputBinding;
	bool Input::s_UseKeyboard = true;
	ControllerId Input::s_LastUsedController = ControllerId::None;

	static GLFWwindow* GetWindow() {
		return Engine::GetWindow()->GetWindowHandle();
	}

	void Input::Init() {
		if (Engine::GetSettings().Headless) return;

		String filePrefix = FileSystem::GetConfigDir() + Engine::GetSettings().Prefix;
		s_InputBinding.Deserize(filePrefix + "KeyboardBindings.ini",
							filePrefix + "ControllerBindings.ini");

		Engine::SaveConfigEvent += [filePrefix]() {
			Input::s_InputBinding.Serialize(filePrefix + "KeyboardBindings.ini", filePrefix + "ControllerBindings.ini");
		};
	}

	void Input::Update() {
		PIT_PROFILE_FUNCTION();

		if (Engine::GetSettings().Headless) return;

		for (int controller = 0; controller < MAX_CONTROLLER_COUNT; controller++) {
			if (glfwJoystickPresent(controller)) {
				if (!s_ControllerActiveStates[controller]) {
					PIT_ENGINE_INFO(General, "New Controller connected: ID{}: {}", controller, glfwGetJoystickName(controller));
					s_ControllerActiveStates[controller] = true;
				}

				int numAxes;
				const float* axises = glfwGetJoystickAxes(controller, &numAxes);
				for (int i = 0; i < numAxes; i++) {
					s_ControllerAxises[controller][i] = axises[i];
					if (axises[i] > 0.1f || axises[i] < -0.1f) {
						s_UseKeyboard = false;
						s_LastUsedController = Cast<ControllerId>(controller);
					}
				}

				int numButtons;
				const unsigned char* buttons = glfwGetJoystickButtons(controller, &numButtons);
				for (int i = 0; i < numButtons; i++) {
					ButtonState state = Cast<ButtonState>(buttons[i]);
					s_ControllerButtonStateChanged[controller][i] = s_ControllerButtonStates[controller][i] != state;
					s_ControllerButtonStates[controller][i] = state;
					if (buttons[i] == GLFW_PRESS) {
						s_UseKeyboard = false;
						s_LastUsedController = Cast<ControllerId>(controller);
					}
				}
				s_ControllerTypes[controller] = Pit::GetControllerType(glfwGetJoystickName(controller));
			}
			else s_ControllerActiveStates[controller] = false;
		}

		GLFWwindow* window = GetWindow();
		for (int i = MIN_KEY; i < KEY_COUNT; i++) {
			ButtonState state = Cast<ButtonState>(glfwGetKey(window, i));
			s_KeyStateChanged[i] = s_KeyStates[i] != state;
			s_KeyStates[i] = state;
			if (state == ButtonState::Pressed) s_UseKeyboard = true;
		}
		for (int i = 0; i < MOUSEBUTTON_COUNT; i++) {
			bool state = glfwGetMouseButton(window, i) == GLFW_PRESS;
			s_MosueButtonStateChanged[i] = s_MouseButtonStates[i] != state;
			s_MouseButtonStates[i] = state;
			if (state) s_UseKeyboard = true;
		}

		double mouse_x, mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		glm::vec2 currentMousePos = { Cast<float>(mouse_x), Cast<float>(mouse_y) };
		m_LastMousePosDelta = currentMousePos - m_LastMousePos;
		m_LastMousePos = currentMousePos;
		if (m_LastMousePosDelta != glm::vec2{ 0, 0 })
			s_UseKeyboard = true;
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

	const bool Input::IsKeyboardAndMouseInUse() {
		return s_UseKeyboard;
	}

	const bool Input::IsControllerInUse(ControllerId* usedController) {
		if (usedController)
			*usedController = s_LastUsedController;
		return !s_UseKeyboard;
	}

	const bool Input::IsControllerConnected(ControllerId controllerId) {
		int controller = Cast<int>(controllerId);
		return glfwJoystickPresent(Cast<int>(controller));
	}

	const ControllerType Input::GetControllerType(ControllerId controllerId) {
		return s_ControllerTypes[Cast<int>(controllerId)];
	}

	const glm::vec2 Input::GetControllerAxis(ControllerId controllerId, ControllerAxis axis) {
		int controller = Cast<int>(controllerId);
		switch (axis) {
		default:
		case ControllerAxis::None: return glm::vec2{ 0, 0 };
		case ControllerAxis::Left: glm::vec2 out{ s_ControllerAxises[controller][CONTROLLER_LEFT_AXIS_X], s_ControllerAxises[controller][CONTROLLER_LEFT_AXIS_Y] };
								 out.x = Math::Clamp(out.x, -1.f, 1.f);
								 out.y = Math::Clamp(out.y, -1.f, 1.f);
								 if (out.x <= 0.1f && out.x >= -0.1f) out.x = 0;
								 if (out.y <= 0.1f && out.y >= -0.1f) out.y = 0;
								 if (GetControllerType(controllerId) == ControllerType::XboxController) out.y = -out.y;
								 return out;
		case ControllerAxis::Right: out = { s_ControllerAxises[controller][CONTROLLER_RIGHT_AXIS_X], s_ControllerAxises[controller][CONTROLLER_RIGHT_AXIS_Y] };
								  out.x = Math::Clamp(out.x, -1.f, 1.f);
								  out.y = Math::Clamp(out.y, -1.f, 1.f);
								  if (out.x <= 0.1f && out.x >= -0.1f) out.x = 0;
								  if (out.y <= 0.1f && out.y >= -0.1f) out.y = 0;
								  return out;
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
		return s_ControllerButtonStates[controller][GetControllerButton(s_ControllerTypes[controller], button)] == ButtonState::Released;
	}
	const bool Input::IsControllerButtonDown(ControllerId controllerId, ControllerButton button) {
		int controller = Cast<int>(controllerId);
		return s_ControllerButtonStates[controller][GetControllerButton(s_ControllerTypes[controller], button)] == ButtonState::Pressed;
	}
	const bool Input::IsControllerButtonPressed(ControllerId controllerId, ControllerButton button) {
		int controller = Cast<int>(controllerId);
		return s_ControllerButtonStates[controller][GetControllerButton(s_ControllerTypes[controller], button)] == ButtonState::Pressed &&
														    s_ControllerButtonStateChanged[controller][GetControllerButton(s_ControllerTypes[controller], button)];
	}
	const bool Input::IsControllerButtonReleased(ControllerId controllerId, ControllerButton button) {
		int controller = Cast<int>(controllerId);
		return s_ControllerButtonStates[controller][GetControllerButton(s_ControllerTypes[controller], button)] == ButtonState::Released &&
															s_ControllerButtonStateChanged[controller][GetControllerButton(s_ControllerTypes[controller], button)];
	}

	void Input::SetMousePos(const glm::vec2& newPos) {
		glfwSetCursorPos(GetWindow(), newPos.x, newPos.y);
		m_LastMousePos = newPos;
	}

	void Input::SetCursorMode(CursorMode mode) {
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
	}
	
	void Input::AddBinding(const String& name, KeyCode keycode) { s_InputBinding.AddBinding(name, keycode); }
	
	void Input::AddBinding(const String& name, MouseButton mouseButton) { s_InputBinding.AddBinding(name, mouseButton); }

	void Input::AddBinding(const String& name, ControllerButton controllerButton) { s_InputBinding.AddBinding(name, controllerButton); }

	void Input::AddAxisBinding(const String& name, const KeyAxis& axis) { s_InputBinding.AddAxisBinding(name, axis); }
	void Input::AddAxisBinding(const String& name, ControllerAxis axis) { s_InputBinding.AddAxisBinding(name, axis); }
	void Input::AddMouseAxisBinding(const String& name) { s_InputBinding.AddMouseAxisBinding(name); }

	const bool Input::HasBinding(const String& name) { return s_InputBinding.HasBinding(name); }
	const bool Input::HasAxisBinding(const String& name) { return s_InputBinding.HasAxisBinding(name); }

	void Input::SetBinding(const String& name, KeyCode newKeycode) { s_InputBinding.SetBinding(name, newKeycode); }
	
	void Input::SetBinding(const String& name, MouseButton newMouseButton) { s_InputBinding.SetBinding(name, newMouseButton); }

	void Input::SetBinding(const String& name, ControllerButton newControllerButton) { s_InputBinding.SetBinding(name, newControllerButton); }

	void Input::SetAxisBinding(const String& name, const KeyAxis& newAxis) { s_InputBinding.SetAxisBinding(name, newAxis); }
	void Input::SetAxisBinding(const String& name, ControllerAxis newAxis) { s_InputBinding.SetAxisBinding(name, newAxis); }
	void Input::SetMouseAxisBinding(const String& name) { s_InputBinding.SetMouseAxisBinding(name); }
	void Input::RemoveMouseAxisBinding(const String& name) { s_InputBinding.RemoveMouseAxisBinding(name); }
	
	const KeyCode Input::GetBinding(const String& name) { return s_InputBinding.GetBinding(name); }
	
	const MouseButton Input::GetMouseBinding(const String& name) { return s_InputBinding.GetMouseBinding(name); }

	const ControllerButton Input::GetControllerButtonBinding(const String& name) { return s_InputBinding.GetControllerButtonBinding(name); }

	const KeyAxis Input::GetKeyAxisBinding(const String& name) { return s_InputBinding.GetAxisBinding(name); }
	const ControllerAxis Input::GetControllerAxisBinding(const String& name) { return s_InputBinding.GetControllerAxisBinding(name); }
	const bool Input::GetMouseAxisBinding(const String& name) { return s_InputBinding.GetMouseAxisBinding(name); }

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
				return IsControllerButtonUp(ControllerId::ID1, button);
			else
				return true;
		}
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
				return IsControllerButtonDown(ControllerId::ID1, button);
			else
				return false;
		}
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
				return IsControllerButtonPressed(ControllerId::ID1, button);
			else
				return false;
		}
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
				return IsControllerButtonReleased(ControllerId::ID1, button);
			else
				return false;
		}
	}

	const glm::vec2 Input::GetAxisBinding(const String& name) {
		if (s_UseKeyboard) {
			if (s_InputBinding.GetMouseAxisBinding(name))
				return Input::GetMouseDelta();
			else {
				KeyAxis axis = GetKeyAxisBinding(name);
				glm::vec2 output{ 0, 0 };
				if (IsKeyDown(axis.Up)) output.y++;
				if (IsKeyDown(axis.Down)) output.y--;
				if (IsKeyDown(axis.Left)) output.x--;
				if (IsKeyDown(axis.Right)) output.x++;
				return output;
			}
		}
		else
			return GetControllerAxis(ControllerId::ID1, GetControllerAxisBinding(name));
	}
	
	bool Input::RemoveBinding(const String& name) { return s_InputBinding.RemoveBinding(name); }
	bool Input::RemoveAxisBinding(const String& name) { return s_InputBinding.RemoveAxisBinding(name); }
}