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
		static bool IsKeyUp(KeyCode key);
		// Is Key down
		static bool IsKeyDown(KeyCode key);
		// Is Key clicked this frame
		static bool IsKeyPressed(KeyCode key);
		// Is Key released this frame
		static bool IsKeyReleased(KeyCode key);
		// Get State of key
		static KeyState GetKey(KeyCode key);

		// Is MouseButton up
		static bool IsMouseButtonUp(MouseButton button);
		// Is MouseButton clicked this frame
		static bool IsMouseButtonDown(MouseButton button);
		// Is MouseButton released this frame
		static bool IsMouseButtonPressed(MouseButton button);
		// Get State of MouseButton
		static bool IsMouseButtonReleased(MouseButton button);

		// Set position of mouse
		static void SetMousePos(const glm::vec2& newPos);
		// Get current position of mouse
		static const glm::vec2& GetMousePos() { return m_LastMousePos; }
		// Get current change in position of mouse
		static const glm::vec2& GetMouseDelta() { return m_LastMousePosDelta; }

		// Set CursorMode of the mouse
		static void SetCursorMode(CursorMode mode);

		static void AddBinding(const String& name, KeyCode keycode) { s_Bindings.AddBinding(name, keycode); }
		static void AddBinding(const String& name, MouseButton mouseButton) { s_Bindings.AddBinding(name, mouseButton); }
		static void SetBinding(const String& name, KeyCode newKeycode) { s_Bindings.SetBinding(name, newKeycode); }
		static void SetBinding(const String& name, MouseButton newMouseButton) { s_Bindings.SetBinding(name, newMouseButton); }
		static KeyCode GetBinding(const String& name) { return s_Bindings.GetBinding(name); }
		static MouseButton GetMouseBinding(const String& name) { return s_Bindings.GetMouseBinding(name); }
		static bool RemoveBinding(const String& name) { return s_Bindings.RemoveBinding(name); }

		static glm::vec2 m_LastMousePos;
		static glm::vec2 m_LastMousePosDelta;
	private:
		static constexpr int KEY_COUNT = 348;
		static constexpr int MIN_KEY = 32;
		static constexpr int MOUSEBUTTON_COUNT = 6;
		static KeyState s_KeyStates[KEY_COUNT];
		static bool s_KeyStateChanged[KEY_COUNT];
		static bool s_MouseButtonStates[MOUSEBUTTON_COUNT];
		static bool s_MosueButtonStateChanged[MOUSEBUTTON_COUNT];
		static Keybindings s_Bindings;
	};
}