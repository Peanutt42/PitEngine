#include "pch.hpp"
#include "Core/Engine.hpp"
#include "RenderingSubmodule.hpp"

using namespace Pit;

static void GLFWErrorCallback(int errorCode, const char* description) {
	PIT_ENGINE_ERR(Log::Rendering, "[GLFW]: {0:d}: {1:s}", errorCode, description);
}

void RenderingSubmodule::Init() {
	glfwInit();
	glfwSetErrorCallback(GLFWErrorCallback);

	Window = new Rendering::Window("PitEngine", 800, 600, true);
	glfwMakeContextCurrent(Window->GetWindowHandle());
	glfwSwapInterval(Engine::GetInfo().VSync);

	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to initialize GLAD");

	Window->SetViewport(Window->GetWidth(), Window->GetHeight());
	Renderer = new Rendering::Renderer();
	UIRenderer = new UI::UIRenderer();

	PIT_ENGINE_INFO(Log::Rendering, "OpenGL Info:");
	PIT_ENGINE_INFO(Log::Rendering, "  Vendor: {0:s}", (const char*)glGetString(GL_VENDOR));
	PIT_ENGINE_INFO(Log::Rendering, "  Renderer: {0:s}", (const char*)glGetString(GL_RENDERER));
	PIT_ENGINE_INFO(Log::Rendering, "  Version: {0:s}", (const char*)glGetString(GL_VERSION));
}

void RenderingSubmodule::Shutdown() {
	delete UIRenderer;
	delete Renderer;
	delete Window;

	glfwTerminate();
}

void RenderingSubmodule::Update() {
	Renderer->Update();
	UIRenderer->Update();
	Window->Update();
}