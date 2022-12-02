#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"
#include "Asset.hpp"

namespace Pit {
	/// <summary>
	/// Handles (Un-)Loading of any asset
	/// </summary>
	class AssetManagmentSubmodule : public Submodule {
	public:
		virtual void Init();

		virtual void Shutdown();

		virtual void Update();

		Array<AssetManagment::Asset*>& GetAssets() { return m_Assets; }

	private:
		Array<AssetManagment::Asset*> m_Assets;
	};
}