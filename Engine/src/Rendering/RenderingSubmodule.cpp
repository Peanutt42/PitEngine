#include "pch.hpp"
#include "Core/Engine.hpp"
#include "RenderingSubmodule.hpp"
#include "Camera.hpp"

namespace Pit {
	void RenderingSubmodule::Init() {
		PIT_PROFILE_FUNCTION();

		glfwInit();

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

		Camera.Release();

		UIRenderer.Release();
		Renderer.Release();
		Window.Release();

		glfwTerminate();
	}

	void RenderingSubmodule::Update() {
		PIT_PROFILE_FUNCTION();

		Renderer->Update();
		UIRenderer->Update();
		Window->Update();
	}
}