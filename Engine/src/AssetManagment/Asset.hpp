#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit::AssetManagment {
#define NULL_ASSET_TYPE 0

	/// <summary>
	/// Base Type for Asset. Examples: Texture, Audio, Mesh, Setting.
	/// When making a new Asset type, remember to add a #define ..._ASSET_TYPE 123
	/// and set m_TypeId to it
	/// </summary>
	class Asset {
	public:
		Asset() : m_Filepath(""), m_TypeId(NULL_ASSET_TYPE) {}
		virtual ~Asset() {}

		virtual void Load() = 0;
		virtual void Unload() = 0;

		const UUID& GetUUID() const { return m_UUID; }
		const bool IsLoaded() const { return m_Loaded; }
		const uint32 GetType() const { return m_TypeId; }
		const std::filesystem::path& GetPath() const { return m_Filepath; }

	protected:
		UUID m_UUID;
		std::filesystem::path m_Filepath;
		uint32 m_TypeId;
		bool m_Loaded;
	};
}