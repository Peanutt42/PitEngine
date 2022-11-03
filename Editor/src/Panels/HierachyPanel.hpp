// Help from TheCherno (Hazel) https://github.com/TheCherno/Hazel/blob/master/Hazelnut/src/Panels/SceneHierarchyPanel.h, see SceneHierachyPanel

#pragma once

#include "EditorWindowPanel.hpp"
#include <entt.hpp>

namespace Pit::Editor {
	class HierachyPanel : public EditorWindowPanel {
	public:

		virtual void OnCreate() override;
		virtual void OnDestroy() override;

		virtual void OnGui() override;

		static entt::entity s_SelectedEntity;

	private:
		void _DrawEntityNode(entt::entity entity);
	};
}