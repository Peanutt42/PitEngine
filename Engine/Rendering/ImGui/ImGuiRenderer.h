#pragma once

#include "Main/CoreInclude.h"
#include "Rendering/RenderingInclude.h"

namespace Pit::Rendering{
	class ImGuiRenderer {
	public:
		ImGuiRenderer(GLFWwindow* window);
		~ImGuiRenderer();

		void NewFrame();
		void Render();
		void _TestWindow() {
			//ImGui::Begin("Test");
			//ImGui::Text("Hello World!");
			//ImGui::End();
		}

	private:
		GLFWwindow* m_Window;
	};
}