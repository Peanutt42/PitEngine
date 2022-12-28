#pragma once

#include "KeyCode.hpp"
#include "Keybinding.hpp"
#include <glm/vec2.hpp>

namespace Pit {	
	/// <summary>
	/// Get or Set Input
	/// </summary>
	class Input {
	public:
		static void Init();

		static void Update();

		// Is Key up
		static const bool IsKeyUp(KeyCode key);
		// Is Key down
		static const bool IsKeyDown(KeyCode key);
		// Is Key clicked this frame
		static const bool IsKeyPressed(KeyCode key);
		// Is Key released this frame
		static const bool IsKeyReleased(KeyCode key);
		// Get State of key
		static const ButtonState GetKey(KeyCode key);

		// Is MouseButton up
		static const bool IsMouseButtonUp(MouseButton button);
		// Is MouseButton clicked this frame
		static const bool IsMouseButtonDown(MouseButton button);
		// Is MouseButton released this frame
		static const bool IsMouseButtonPressed(MouseButton button);
		// Get State of MouseButton
		static const bool IsMouseButtonReleased(MouseButton button);


		static const bool IsControllerConnected(ControllerId controllerId);

		static const glm::vec2 GetControllerAxis(ControllerId controllerId, ControllerAxis axis);
		static const float GetControllerPedal(ControllerId controllerId, ControllerPedal pedal);

		static const bool IsControllerButtonUp(ControllerId controllerId, ControllerButton button);
		static const bool IsControllerButtonDown(ControllerId controllerId, ControllerButton button);
		static const bool IsControllerButtonPressed(ControllerId controllerId, ControllerButton button);
		static const bool IsControllerButtonReleased(ControllerId controllerId, ControllerButton button);

		// Set position of mouse
		static void SetMousePos(const glm::vec2& newPos);
		// Get current position of mouse
		static const glm::vec2& GetMousePos() { return m_LastMousePos; }
		// Get current change in position of mouse
		static const glm::vec2& GetMouseDelta() { return m_LastMousePosDelta; }

		// Set CursorMode of the mouse
		static void SetCursorMode(CursorMode mode);

		static void AddBinding(const String& name, KeyCode keycode);
		static void AddBinding(const String& name, MouseButton mouseButton);
		static void AddBinding(const String& name, ControllerButton controllerButton);
		static void SetBinding(const String& name, KeyCode newKeycode);
		static void SetBinding(const String& name, MouseButton newMouseButton);
		static void SetBinding(const String& name, ControllerButton newControllerButton);
		static const KeyCode GetBinding(const String& name);
		static const MouseButton GetMouseBinding(const String& name);
		static const ControllerButton GetControllerButtonBinding(const String& name);
		static const bool IsBindingUp(const String& name);
		static const bool IsBindingDown(const String& name);
		static const bool IsBindingPressed(const String& name);
		static const bool IsBindingReleased(const String& name);
		static bool RemoveBinding(const String& name);

		static glm::vec2 m_LastMousePos;
		static glm::vec2 m_LastMousePosDelta;
	private:
#define KEY_COUNT 348
#define MIN_KEY 32
		static ButtonState s_KeyStates[KEY_COUNT];
		static bool s_KeyStateChanged[KEY_COUNT];
#define MOUSEBUTTON_COUNT 6
		static bool s_MouseButtonStates[MOUSEBUTTON_COUNT];
		static bool s_MosueButtonStateChanged[MOUSEBUTTON_COUNT];


#define MAX_CONTROLLER_COUNT 16
#define CONTROLLER_BUTTON_COUNT 15
		static ButtonState s_ControllerButtonStates[MAX_CONTROLLER_COUNT][CONTROLLER_BUTTON_COUNT];
		static bool s_ControllerButtonStateChanged[MAX_CONTROLLER_COUNT][CONTROLLER_BUTTON_COUNT];

#define CONTROLLER_AXIS_COUNT 6
		static float s_ControllerAxises[MAX_CONTROLLER_COUNT][CONTROLLER_AXIS_COUNT];

		static ControllerType s_CurrentControllerType[MAX_CONTROLLER_COUNT];

		static Keybindings s_Bindings;

		static bool s_UseKeyboard;
	};
}