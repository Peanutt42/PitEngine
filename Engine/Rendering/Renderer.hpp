#pragma once

#include "Main/CoreInclude.hpp"
#include "RenderingInclude.hpp"
#include "ImGui/ImGuiRenderer.hpp"
#include "Window.hpp"

namespace Pit::Rendering {
	struct RendererSpecs {
		bool VSync = false;
	};

	class IRenderer {
	public:
		IRenderer() = default;

		virtual void Render() = 0;

		virtual void SetWindow(Window* window) = 0;
		virtual void SetSpecs(const RendererSpecs& specs) = 0;

		virtual bool ShouldClose() { return false; }

		ImGuiRenderer* ImGuiRenderer;
	};

	class Renderer {
	public:
		Renderer();
		~Renderer();

		void Render();

		void SetWindow(Window* window);
		void SetSpecs(const RendererSpecs& specs);

		bool ShouldClose();

		static IRenderer* Instance;

	private:
		IRenderer* m_Renderer;
	};
}