#pragma once

#include "Main/CoreInclude.hpp"
#include "Main/Submodule.hpp"
#include "Renderer.hpp"
#include "UI/UIRenderer.hpp"

namespace Pit {
	DECLARE_EXTERN_PROFILE_STAT_FLOAT(RenderingUpdate, Rendering, "Rendering-Update");

	class RenderingSubmodule : public Submodule {
	public:
		virtual void Init() {
			Renderer = new Rendering::Renderer();
			UIRenderer = new Rendering::UI::Renderer(Renderer);
		}
		
		virtual void Shutdown() {
			delete UIRenderer;
			delete Renderer;
		}
		
		virtual void Update() {
			SCOPE_STAT(RenderingUpdate);

			Renderer->Update();
		}

		Rendering::Renderer* GetRenderer() { return Renderer; }
		Rendering::UI::Renderer* GetUIRenderer() { return UIRenderer; }

	private:
		Rendering::Renderer* Renderer = nullptr;
		Rendering::UI::Renderer* UIRenderer = nullptr;
	};
}