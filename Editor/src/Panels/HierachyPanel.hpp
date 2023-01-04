// Help from TheCherno (Hazel) https://github.com/TheCherno/Hazel/blob/master/Hazelnut/src/Panels/SceneHierarchyPanel.h, see SceneHierachyPanel

#pragma once

#include "EditorWindowPanel.hpp"
#include "ECS/ECSEntityHandle.hpp"

namespace Pit::Editor {
	class HierachyPanel : public EditorWindowPanel {
	public:

		virtual void OnCreate() override;
		virtual void OnDestroy() override;

		virtual void OnGui() override;

		static ECS::EntityHandle s_SelectedEntity;

	private:
		void _DrawEntityNode(entt::entity entity);
	};
}