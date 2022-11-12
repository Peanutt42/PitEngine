// Help from TheCherno (Hazel) https://github.com/TheCherno/Hazel/blob/master/Hazelnut/src/Panels/SceneHierarchyPanel.h, see SceneHierachyPanel->Property Window

#pragma once

#include "EditorWindowPanel.hpp"
#include "ECS/ECSWorld.hpp"

namespace Pit::Editor {
	class PropertiesPanel : public EditorWindowPanel {
	public:

		virtual void OnCreate() override;
		virtual void OnDestroy() override;

		virtual void OnGui() override;

	private:
		void _DrawComponents(ECS::World* world, entt::entity entity);
	};
}