#pragma once

#include "pch.hpp"
#include "Rendering/Vulkan/VulkanTexture.hpp"

namespace Pit::Editor {
	enum Icon : int {
		FolderIcon = 0,
		TextIcon = 1,
		AudioIcon = 2,
		MeshIcon = 3,
		ImageIcon = 4,
		ShaderIcon = 5,
		BackIcon = 6,
		RefreshIcon = 7
	};

	class EditorAssetManager {
	public:
		void Init(), Shutdown();

		Rendering::Texture* GetIcon(Icon icon) {
			return m_Icons[icon];
		}

	private:
		std::vector<Rendering::Texture*> m_Icons;
	};
}