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
	class RenderingSubmodule : public Submodule {
	public:
		virtual void Init();
		
		virtual void Shutdown();
		
		virtual void Update();

		Rendering::Window* Window = nullptr;
		Rendering::Renderer* Renderer = nullptr;
		UI::UIRenderer* UIRenderer = nullptr;
	};
}