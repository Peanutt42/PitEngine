#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"
#include "Asset.hpp"

namespace Pit {
	class AssetManagmentSubmodule : public Submodule {
	public:
		virtual void Init();

		virtual void Shutdown();

		virtual void Update();

	private:
		Array<AssetManagment::Asset*> m_Assets;
	};
}