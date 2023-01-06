#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"
#include "Rendering/Window.hpp"
#include "Rendering/Renderer.hpp"
#include "UI/UIRenderer.hpp"
#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

namespace Pit {
	/// <summary>
	/// Submodule that handles anything related to Graphics like Window Handling, UI/2D/3D Rendering
	/// </summary>
	class RenderingSubmodule : public Submodule {
	public:
		virtual void Init();
		
		virtual void Shutdown();
		
		virtual void Update();

		ScopeRef<Rendering::Window> Window;
		ScopeRef<Rendering::Renderer> Renderer;
		ScopeRef<UI::UIRenderer> UIRenderer;
		ScopeRef<Rendering::Camera> Camera;
	};
}