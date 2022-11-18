#pragma once

#include "Utils/StringUtils.hpp"

namespace Pit {
	enum class KeyCode : uint16_t {
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

	enum class KeyState : int {
		None = -1,
		Released = 0,
		Pressed = 1,
		Held = 2
	};

	enum class CursorMode {
		Normal = 0,
		Hidden = 1,
		Locked = 2
	};

	enum class MouseButton : uint16_t {
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
}