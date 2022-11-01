#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"
#include "Rendering/Window.hpp"
#include "Rendering/Renderer.hpp"

namespace Pit {
	class RenderingSubmodule : public Submodule {
	public:
		virtual void Init();
		
		virtual void Shutdown();
		
		virtual void Update();

		Rendering::Window* Window = nullptr;
		Rendering::Renderer* Renderer = nullptr;
	};
}