#include "Main/CoreInclude.hpp"
#include "Input.hpp"
#include "Main/Engine.hpp"
#include <GLFW/glfw3.h>

using namespace Pit;

static GLFWwindow* GetWindow() {
	return Engine::Rendering()->GetRenderer()->Window.GetWindowHandle();
}

bool Input::IsKeyDown(KeyCode key) {
	int state = glfwGetKey(GetWindow(), Cast<int>(key));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

KeyState Input::GetKey(KeyCode key) {
	return Cast<KeyState>(glfwGetKey(GetWindow(), Cast<int>(key)));
}

bool Input::IsMouseButtonDown(MouseButton button) {
	int state = glfwGetMouseButton(GetWindow(), (int)button);
	return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition() {
	double x, y;
	glfwGetCursorPos(GetWindow(), &x, &y);
	return { Cast<float>(x), Cast<float>(y) };
}

glm::vec2 Input::GetMousePositionDelta() {
	static double last_x, last_y;
	static double x, y;
	glfwGetCursorPos(GetWindow(), &x, &y);
	glm::vec2 delta{ x - last_x, y - last_y };
	last_x = x;
	last_y = y;
	return delta;
}

void Input::SetCursorMode(CursorMode mode) {
	glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}