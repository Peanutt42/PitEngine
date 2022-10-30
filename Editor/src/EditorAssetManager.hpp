#pragma once

#include "pch.hpp"
#include "Rendering/Texture.hpp"
#include <imgui/imgui.h>

namespace Pit::Editor {
	enum Icon : int {
		FolderIcon = 0,
		TextIcon = 1,
		AudioIcon = 2,
		MeshIcon = 3,
		ImageIcon = 4,
		ShaderIcon = 5,
		BackIcon = 6,
		RefreshIcon = 7,
		OptionsIcon = 8
	};

	class EditorAssetManager {
	public:
		void Init(), Shutdown();

		static ImTextureID GetIcon(Icon icon);
		static const Rendering::Texture* GetIconTexture(Icon icon);
	
	private:
		Array<Rendering::Texture*> m_Icons;
	};

}