#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

enum KeyCode : uint16_t {
	A = GLFW_KEY_A,
	B = GLFW_KEY_B,
	C = GLFW_KEY_C,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	I = GLFW_KEY_I,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,
	M = GLFW_KEY_M,
	N = GLFW_KEY_N,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,
	Q = GLFW_KEY_Q,
	R = GLFW_KEY_R,
	S = GLFW_KEY_S,
	T = GLFW_KEY_T,
	U = GLFW_KEY_U,
	V = GLFW_KEY_V,
	W = GLFW_KEY_W,
	X = GLFW_KEY_X,
	Y = GLFW_KEY_Y,
	Z = GLFW_KEY_Z,

	Left = GLFW_KEY_LEFT,
	Right = GLFW_KEY_RIGHT,
	Up = GLFW_KEY_UP,
	Down = GLFW_KEY_DOWN,

	Space = GLFW_KEY_SPACE,
	LeftShift = GLFW_KEY_LEFT_SHIFT,
	RightShift = GLFW_KEY_RIGHT_SHIFT,
};

enum KeyState {
	None = -1,
	Pressed,
	Held,
	Released
};

enum MouseButton : uint16_t {
	Button0 = GLFW_MOUSE_BUTTON_1,
	Button1 = GLFW_MOUSE_BUTTON_2,
	Button2 = GLFW_MOUSE_BUTTON_3,
	Button3 = GLFW_MOUSE_BUTTON_4,
	Button4 = GLFW_MOUSE_BUTTON_5,
	Button5 = GLFW_MOUSE_BUTTON_6,
	LeftButton = GLFW_MOUSE_BUTTON_1, // Button0
	RightButton = GLFW_MOUSE_BUTTON_2, // Button1
	MiddleButton = GLFW_MOUSE_BUTTON_3 // Button2
};

enum CursorMode {
	Normal = 0,
	Hidden = 1,
	Locked = 2
};