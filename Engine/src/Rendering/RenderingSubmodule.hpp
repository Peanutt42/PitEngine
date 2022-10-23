#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"
#include "Renderer.hpp"
#include "RenderingSystem.hpp"
#include "Camera.hpp"
#include "UI/UIRenderer.hpp"

namespace Pit {
	class RenderingSubmodule : public Submodule {
	public:
		virtual void Init();
		
		virtual void Shutdown();
		
		virtual void Update();

		Rendering::Renderer* Renderer = nullptr;
		Rendering::RenderingSystem* RenderingSystem = nullptr;
		Rendering::Camera* CurrentCamera = nullptr;
		Pit::UI::Renderer* UIRenderer = nullptr;
	};
}