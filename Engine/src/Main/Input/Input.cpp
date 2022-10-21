#include "Main/CoreInclude.hpp"
#include "Main/Engine.hpp"
#include "Input.hpp"
#include "Main/Engine.hpp"
#include <GLFW/glfw3.h>

using namespace Pit;

glm::vec2 Input::m_LastMousePos;
glm::vec2 Input::m_LastMousePosDelta;

static GLFWwindow* GetWindow() {
	return Engine::Rendering()->Renderer->Window.GetWindowHandle();
}

void Input::Update() {

	auto currentMousePos = _GetMousePosition();
	m_LastMousePosDelta = currentMousePos - m_LastMousePos;
	m_LastMousePos = currentMousePos;
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

glm::vec2 Input::_GetMousePosition() {
	double x, y;
	glfwGetCursorPos(GetWindow(), &x, &y);
	return { Cast<float>(x), Cast<float>(y) };
}

glm::vec2 Input::GetMousePositionDelta() {
	return m_LastMousePosDelta;
}

void Input::SetCursorMode(CursorMode mode) {
	glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}