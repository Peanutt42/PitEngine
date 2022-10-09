// Help from TheCherno (Hazel) , see SceneHierachyPanel->Property Window

#pragma once

#include "Engine/Rendering/UI/UILayer.hpp"
#include "Engine/ECS/ECSWorld.hpp"

namespace Pit::Editor {
	class InspectorPanel : public Rendering::UI::Layer {
	public:

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnGUI() override;

	private:
		void _DrawComponents(ECS::World* world, entt::entity entity);
	};
}