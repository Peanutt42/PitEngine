#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit::AssetManagment {
	class Asset {
	public:
		Asset() : m_Filepath("") {}
		virtual ~Asset() {}

		virtual void Load() = 0;
		virtual void Unload() = 0;

		UUID& GetUUID() { return m_UUID; }
		bool IsLoaded() { return m_Loaded; }
		std::filesystem::path& GetPath() { return m_Filepath; }

	protected:
		UUID m_UUID;
		std::filesystem::path m_Filepath;
		bool m_Loaded;
	};
}