// Help from TheCherno (Hazel) https://github.com/TheCherno/Hazel/blob/master/Hazelnut/src/Panels/SceneHierarchyPanel.h, see SceneHierachyPanel

#pragma once

#include "Engine/Rendering/UI/UILayer.hpp"
#include <entt.hpp>

namespace Pit::Editor {
	class HierachyPanel : public Rendering::UI::Layer {
	public:

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnGUI() override;

		static entt::entity s_SelectedEntity;

	private:
		void _DrawEntityNode(entt::entity entity);
	};
}