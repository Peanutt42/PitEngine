#pragma once

#include "Utils/StringUtils.hpp"

namespace Pit {
	/// <summary>
	/// Every Keyboard Key as a special code
	/// </summary>
	enum class KeyCode : uint16_t {
		Null = 0,
		A = 65,					// GLFW_KEY_A
		B = 66,					// GLFW_KEY_B
		C = 67,					// GLFW_KEY_C
		D = 68,					// GLFW_KEY_D
		E = 69,					// GLFW_KEY_E
		F = 70,					// GLFW_KEY_F
		G = 71,					// GLFW_KEY_G
		H = 72,					// GLFW_KEY_H
		I = 73,					// GLFW_KEY_I
		J = 74,					// GLFW_KEY_J
		K = 75,					// GLFW_KEY_K
		L = 76,					// GLFW_KEY_L
		M = 77,					// GLFW_KEY_M
		N = 78,					// GLFW_KEY_N
		O = 79,					// GLFW_KEY_O
		P = 80,					// GLFW_KEY_P
		Q = 81,					// GLFW_KEY_Q
		R = 82,					// GLFW_KEY_R
		S = 83,					// GLFW_KEY_S
		T = 84,					// GLFW_KEY_T
		U = 85,					// GLFW_KEY_U
		V = 86,					// GLFW_KEY_V
		W = 87,					// GLFW_KEY_W
		X = 88,					// GLFW_KEY_X
		Y = 89,					// GLFW_KEY_Y
		Z = 90,					// GLFW_KEY_Z

		Number_0 = 48,			// GLFW_KEY_0
		Number_1 = 49,			// GLFW_KEY_1
		Number_2 = 50,			// GLFW_KEY_2
		Number_3 = 51,			// GLFW_KEY_3
		Number_4 = 52,			// GLFW_KEY_4
		Number_5 = 53,			// GLFW_KEY_5
		Number_6 = 54,			// GLFW_KEY_6
		Number_7 = 55,			// GLFW_KEY_7
		Number_8 = 56,			// GLFW_KEY_8
		Number_9 = 57,			// GLFW_KEY_9

		Space = 32,				// GLFW_KEY_SPACE
		Escape = 256,			// GLFW_KEY_ESCAPE
		Tab = 258,				// GLFW_KEY_TAB
		LeftShift = 340,		// GLFW_KEY_LEFT_SHIFT
		RightShift = 344,		// GLFW_KEY_RIGHT_SHIFT
		LeftControl = 341,		// GLFW_KEY_LEFT_CONTROL
		RightControl = 345,		// GLFW_KEY_RIGHT_CONTROL
		Windows = 343,			// GLFW_KEY_LEFT_SUPER
		Alt = 342,				// GLFW_KEY_LEFT_ALT
		AltGr = 346,			// GLFW_KEY_RIGHT_ALT
		Enter = 257,			// GLFW_KEY_ENTER
		Delete = 261,			// GLFW_KEY_DELETE

		Arrow_Left = 263,		// GLFW_KEY_LEFT
		Arrow_Right = 262,		// GLFW_KEY_RIGHT
		Arrow_Up = 265,			// GLFW_KEY_UP
		Arrow_Down = 264,		// GLFW_KEY_DOWN

		F1 = 290,				// GLFW_KEY_F1
		F2 = 291,				// GLFW_KEY_F2
		F3 = 292,				// GLFW_KEY_F3
		F4 = 293,				// GLFW_KEY_F4
		F5 = 294,				// GLFW_KEY_F5
		F6 = 295,				// GLFW_KEY_F6
		F7 = 296,				// GLFW_KEY_F7
		F8 = 297,				// GLFW_KEY_F8
		F9 = 298,				// GLFW_KEY_F9
		F10 = 299,				// GLFW_KEY_F10
		F11 = 300,				// GLFW_KEY_F11
		F12 = 301,				// GLFW_KEY_F12
	};

#pragma region ToString/FromString
	static String KeyCodeToString(KeyCode code) {
		switch (code) {
		case KeyCode::Null: return "Null";

		case KeyCode::A: return "A";
		case KeyCode::B: return "B";
		case KeyCode::C: return "C";
		case KeyCode::D: return "D";
		case KeyCode::E: return "E";
		case KeyCode::F: return "F";
		case KeyCode::G: return "G";
		case KeyCode::H: return "H";
		case KeyCode::I: return "I";
		case KeyCode::J: return "J";
		case KeyCode::K: return "K";
		case KeyCode::L: return "L";
		case KeyCode::M: return "M";
		case KeyCode::N: return "N";
		case KeyCode::O: return "O";
		case KeyCode::P: return "P";
		case KeyCode::Q: return "Q";
		case KeyCode::R: return "R";
		case KeyCode::S: return "S";
		case KeyCode::T: return "T";
		case KeyCode::U: return "U";
		case KeyCode::V: return "V";
		case KeyCode::W: return "W";
		case KeyCode::X: return "X";
		case KeyCode::Y: return "Y";
		case KeyCode::Z: return "Z";

		case KeyCode::Number_0: return "Number_0";
		case KeyCode::Number_1: return "Number_1";
		case KeyCode::Number_2: return "Number_2";
		case KeyCode::Number_3: return "Number_3";
		case KeyCode::Number_4: return "Number_4";
		case KeyCode::Number_5: return "Number_5";
		case KeyCode::Number_6: return "Number_6";
		case KeyCode::Number_7: return "Number_7";
		case KeyCode::Number_8: return "Number_8";
		case KeyCode::Number_9: return "Number_9";

		case KeyCode::Space:		return "Space";
		case KeyCode::Escape:		return "Escape";
		case KeyCode::Tab:			return "Tab";
		case KeyCode::LeftShift:	return "LeftShift";
		case KeyCode::RightShift:	return "RightShift";
		case KeyCode::LeftControl:	return "LeftControl";
		case KeyCode::RightControl: return "RightControl";
		case KeyCode::Windows:		return "Windows";
		case KeyCode::Alt:			return "Alt";
		case KeyCode::AltGr:		return "AltGr";
		case KeyCode::Enter:		return "Enter";
		case KeyCode::Delete:		return "Delete";

		case KeyCode::Arrow_Left:	return "Arrow_Left";
		case KeyCode::Arrow_Right:	return "Arrow_Right";
		case KeyCode::Arrow_Up:		return "Arrow_Up";
		case KeyCode::Arrow_Down:	return "Arrow_Down";

		case KeyCode::F1: return "F1";
		case KeyCode::F2: return "F2";
		case KeyCode::F3: return "F3";
		case KeyCode::F4: return "F4";
		case KeyCode::F5: return "F5";
		case KeyCode::F6: return "F6";
		case KeyCode::F7: return "F7";
		case KeyCode::F8: return "F8";
		case KeyCode::F9: return "F9";
		case KeyCode::F10: return "F10";
		case KeyCode::F11: return "F11";
		case KeyCode::F12: return "F12";
		}
		return "";
	}

	static KeyCode StringToKeyCode(const String& string) {
		if (string == "Null") return KeyCode::Null;

		if (string == "A") return KeyCode::A;
		if (string == "B") return KeyCode::B;
		if (string == "C") return KeyCode::C;
		if (string == "D") return KeyCode::D;
		if (string == "E") return KeyCode::E;
		if (string == "F") return KeyCode::F;
		if (string == "G") return KeyCode::G;
		if (string == "H") return KeyCode::H;
		if (string == "I") return KeyCode::I;
		if (string == "J") return KeyCode::J;
		if (string == "K") return KeyCode::K;
		if (string == "L") return KeyCode::L;
		if (string == "M") return KeyCode::M;
		if (string == "N") return KeyCode::N;
		if (string == "O") return KeyCode::O;
		if (string == "P") return KeyCode::P;
		if (string == "Q") return KeyCode::Q;
		if (string == "R") return KeyCode::R;
		if (string == "S") return KeyCode::S;
		if (string == "T") return KeyCode::T;
		if (string == "U") return KeyCode::U;
		if (string == "V") return KeyCode::V;
		if (string == "W") return KeyCode::W;
		if (string == "X") return KeyCode::X;
		if (string == "Y") return KeyCode::Y;
		if (string == "Z") return KeyCode::Z;

		if (string == "Number_0") return KeyCode::Number_0;
		if (string == "Number_1") return KeyCode::Number_1;
		if (string == "Number_2") return KeyCode::Number_2;
		if (string == "Number_3") return KeyCode::Number_3;
		if (string == "Number_4") return KeyCode::Number_4;
		if (string == "Number_5") return KeyCode::Number_5;
		if (string == "Number_6") return KeyCode::Number_6;
		if (string == "Number_7") return KeyCode::Number_7;
		if (string == "Number_8") return KeyCode::Number_8;
		if (string == "Number_9") return KeyCode::Number_9;

		if (string == "Space") return KeyCode::Space;
		if (string == "Escape") return KeyCode::Escape;
		if (string == "Tab") return KeyCode::Tab;
		if (string == "LeftShift") return KeyCode::LeftShift;
		if (string == "RightShift") return KeyCode::RightShift;
		if (string == "LeftControl") return KeyCode::LeftControl;
		if (string == "RightControl") return KeyCode::RightControl;
		if (string == "Windows") return KeyCode::Windows;
		if (string == "Alt") return KeyCode::Alt;
		if (string == "AltGr") return KeyCode::AltGr;
		if (string == "Enter") return KeyCode::Enter;
		if (string == "Delete") return KeyCode::Delete;

		if (string == "Arrow_Left") return KeyCode::Arrow_Left;
		if (string == "Arrow_Right") return KeyCode::Arrow_Right;
		if (string == "Arrow_Up") return KeyCode::Arrow_Up;
		if (string == "Arrow_Down") return KeyCode::Arrow_Down;

		if (string == "F1") return KeyCode::F1;
		if (string == "F2") return KeyCode::F2;
		if (string == "F3") return KeyCode::F3;
		if (string == "F4") return KeyCode::F4;
		if (string == "F5") return KeyCode::F5;
		if (string == "F6") return KeyCode::F6;
		if (string == "F7") return KeyCode::F7;
		if (string == "F8") return KeyCode::F8;
		if (string == "F9") return KeyCode::F9;
		if (string == "F10") return KeyCode::F10;
		if (string == "F11") return KeyCode::F11;
		if (string == "F12") return KeyCode::F12;

		return KeyCode::Null;
	}
#pragma endregion

	/// <summary>
	/// Any state a key could be in
	/// </summary>
	enum class KeyState : int {
		None = -1,
		Released = 0,
		Pressed = 1,
		Held = 2
	};

	/// <summary>
	/// Diffrent Cursor Modes
	/// </summary>
	enum class CursorMode {
		Normal = 0,
		Hidden = 1,
		Locked = 2
	};

	/// <summary>
	/// Every supported mouse button
	/// </summary>
	enum class MouseButton : uint16_t {
		Null = (uint16_t)-1,
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Left = 0,				// Button0
		Right = 1,				// Button1
		Middle = 2				// Button2
	};

#pragma region ToString/FromString
	static String MouseButtonToString(MouseButton button) {
		switch (button) {
		case MouseButton::Null:		return "Null";
		case MouseButton::Left:		return "Left";
		case MouseButton::Right:	return "Right";
		case MouseButton::Middle:	return "Middle";
		case MouseButton::Button3:	return "Button3";
		case MouseButton::Button4:	return "Button4";
		case MouseButton::Button5:	return "Button5";
		}
		return "Null";
	}

	static MouseButton StringToMouseButton(const String& string) {
		if (string == "Null")		return MouseButton::Null;
		if (string == "Left")		return MouseButton::Left;
		if (string == "Right")		return MouseButton::Right;
		if (string == "Middle")		return MouseButton::Middle;
		if (string == "Button3")	return MouseButton::Button3;
		if (string == "Button4")	return MouseButton::Button4;
		if (string == "Button5")	return MouseButton::Button5;
		return MouseButton::Null;
	}
#pragma endregion
}