#pragma once

#include "KeyCode.hpp"
#include "InputBinding.hpp"
#include <glm/vec2.hpp>

namespace Pit {	
	/// <summary>
	/// Get or Set Input
	/// </summary>
	class Input {
	public:
		static void Init();

		static void Update();

		static const bool IsKeyUp(KeyCode key);
		static const bool IsKeyDown(KeyCode key);
		static const bool IsKeyPressed(KeyCode key);
		static const bool IsKeyReleased(KeyCode key);
		static const ButtonState GetKey(KeyCode key);

		static const bool IsMouseButtonUp(MouseButton button);
		static const bool IsMouseButtonDown(MouseButton button);
		static const bool IsMouseButtonPressed(MouseButton button);
		static const bool IsMouseButtonReleased(MouseButton button);

		static const bool IsKeyboardAndMouseInUse();
		static const bool IsControllerInUse(ControllerId* usedController = nullptr);

		static const bool IsControllerConnected(ControllerId controllerId);
		static const ControllerType GetControllerType(ControllerId controllerId);

		static const glm::vec2 GetControllerAxis(ControllerId controllerId, ControllerAxis axis);
		static const float GetControllerPedal(ControllerId controllerId, ControllerPedal pedal);

		static const bool IsControllerButtonUp(ControllerId controllerId, ControllerButton button);
		static const bool IsControllerButtonDown(ControllerId controllerId, ControllerButton button);
		static const bool IsControllerButtonPressed(ControllerId controllerId, ControllerButton button);
		static const bool IsControllerButtonReleased(ControllerId controllerId, ControllerButton button);

		static void SetMousePos(const glm::vec2& newPos);
		static const glm::vec2& GetMousePos() { return m_LastMousePos; }
		static const glm::vec2& GetMouseDelta() { return m_LastMousePosDelta; }

		static void SetCursorMode(CursorMode mode);

		static void AddBinding(const String& name, KeyCode keycode);
		static void AddBinding(const String& name, MouseButton mouseButton);
		static void AddBinding(const String& name, ControllerButton controllerButton);

		static void AddAxisBinding(const String& name, const KeyAxis& axis);
		static void AddAxisBinding(const String& name, ControllerAxis axis);
		static void AddMouseAxisBinding(const String& name);

		static const bool HasBinding(const String& name);
		static const bool HasAxisBinding(const String& name);

		static void SetBinding(const String& name, KeyCode newKeycode);
		static void SetBinding(const String& name, MouseButton newMouseButton);
		static void SetBinding(const String& name, ControllerButton newControllerButton);

		static void SetAxisBinding(const String& name, const KeyAxis& newAxis);
		static void SetAxisBinding(const String& name, ControllerAxis newAxis);
		static void SetMouseAxisBinding(const String& name);
		static void RemoveMouseAxisBinding(const String& name);

		static const KeyCode GetBinding(const String& name);
		static const MouseButton GetMouseBinding(const String& name);
		static const ControllerButton GetControllerButtonBinding(const String& name);

		static const KeyAxis GetKeyAxisBinding(const String& name);
		static const ControllerAxis GetControllerAxisBinding(const String& name);
		static const bool GetMouseAxisBinding(const String& name);

		static const bool IsBindingUp(const String& name);
		static const bool IsBindingDown(const String& name);
		static const bool IsBindingPressed(const String& name);
		static const bool IsBindingReleased(const String& name);

		static const glm::vec2 GetAxisBinding(const String& name);

		static bool RemoveBinding(const String& name);
		static bool RemoveAxisBinding(const String& name);

	private:
		static glm::vec2 m_LastMousePos;
		static glm::vec2 m_LastMousePosDelta;

#define KEY_COUNT 348
#define MIN_KEY 32
		static ButtonState s_KeyStates[KEY_COUNT];
		static bool s_KeyStateChanged[KEY_COUNT];
#define MOUSEBUTTON_COUNT 6
		static bool s_MouseButtonStates[MOUSEBUTTON_COUNT];
		static bool s_MosueButtonStateChanged[MOUSEBUTTON_COUNT];


#define MAX_CONTROLLER_COUNT 16
#define CONTROLLER_BUTTON_COUNT 14
		static ButtonState s_ControllerButtonStates[MAX_CONTROLLER_COUNT][CONTROLLER_BUTTON_COUNT];
		static bool s_ControllerButtonStateChanged[MAX_CONTROLLER_COUNT][CONTROLLER_BUTTON_COUNT];

#define CONTROLLER_AXIS_COUNT 6
		static float s_ControllerAxises[MAX_CONTROLLER_COUNT][CONTROLLER_AXIS_COUNT];

		static ControllerType s_ControllerTypes[MAX_CONTROLLER_COUNT];

		static InputBinding s_InputBinding;

		static bool s_UseKeyboard;
		static ControllerId s_LastUsedController;
	};
}