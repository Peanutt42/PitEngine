#pragma once

#include "Main/CoreInclude.hpp"
#include "Main/Submodule.hpp"
#include "Renderer.hpp"
#include "UI/UIRenderer.hpp"

namespace Pit {
	class RenderingSubmodule : public Submodule {
	public:
		virtual void Init() {
			Renderer = new Rendering::Renderer();
			UIRenderer = new UI::Renderer(Renderer);
		}
		
		virtual void Shutdown() {
			delete UIRenderer;
			delete Renderer;
		}
		
		virtual void Update() {
			Renderer->Update();
		}

		Rendering::Renderer* GetRenderer() { return Renderer; }
		Pit::UI::Renderer* GetUIRenderer() { return UIRenderer; }

	private:
		Rendering::Renderer* Renderer = nullptr;
		Pit::UI::Renderer* UIRenderer = nullptr;
	};
}