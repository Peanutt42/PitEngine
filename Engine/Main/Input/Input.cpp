#include "Input.hpp"
#include "Rendering/Window.hpp"

using namespace Pit;

bool Input::IsKeyDown(KeyCode keycode) {
	int state = glfwGetKey(Rendering::Window::CurrentlySelectedWindow, (int)keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonDown(MouseButton button) {
	int state = glfwGetMouseButton(Rendering::Window::CurrentlySelectedWindow, (int)button);
	return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition() {
	double x, y;
	glfwGetCursorPos(Rendering::Window::CurrentlySelectedWindow, &x, &y);
	return { (float)x, (float)y };
}

void Input::SetCursorMode(CursorMode mode) {
	glfwSetInputMode(Rendering::Window::CurrentlySelectedWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}