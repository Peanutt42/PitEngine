#include "pch.hpp"
#include "Core/Engine.hpp"
#include "RenderingSubmodule.hpp"
#include "Camera.hpp"

namespace Pit {

	static void GLFWErrorCallback([[maybe_unused]] int errorCode, [[maybe_unused]] const char* description) {
		PIT_ENGINE_ERR(Rendering, "[GLFW]: {0:d}: {1:s}", errorCode, description);
	}

	void RenderingSubmodule::Init() {
		PIT_PROFILE_FUNCTION();

		glfwInit();
		glfwSetErrorCallback(GLFWErrorCallback);

		Window = new Rendering::Window(Engine::GetSettings().WindowName, 1920, 1080, false);
		glfwMakeContextCurrent(Window->GetWindowHandle());
		glfwSwapInterval(Engine::GetSettings().VSync);

		if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
			PIT_ENGINE_FATAL(Rendering, "Failed to initialize GLAD");

		Window->SetViewport(Window->GetWidth(), Window->GetHeight());
		Renderer = new Rendering::Renderer();
		UIRenderer = new UI::UIRenderer();

		Camera = new Rendering::Camera({ 0.f, 0.f, 5.f });
	}

	void RenderingSubmodule::Shutdown() {
		PIT_PROFILE_FUNCTION();

		delete Camera;

		delete UIRenderer;
		delete Renderer;
		delete Window;

		glfwTerminate();
	}

	void RenderingSubmodule::Update() {
		PIT_PROFILE_FUNCTION();

		Renderer->Update();
		UIRenderer->Update();
		Window->Update();
	}
}