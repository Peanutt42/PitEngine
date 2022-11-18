#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/EngineSettings.hpp"

#include <yaml-cpp/yaml.h>

namespace Pit::Serialization {
	class EngineSerializer {
	public:
		static void Serialize(const String& filepath, EngineSettings& settings) {
			using namespace YAML;
			Emitter out;
			out << BeginMap;

			out << Comment("EngineSettings");
			out << Key << "WindowName" << Value << settings.WindowName;
			out << Key << "WindowToolbar" << Value << settings.WindowToolbar;
			out << Key << "VSync" << Value << settings.VSync;
			out << Key << "UIDocking" << Value << settings.UIDocking;
			out << Key << "RenderingApi" << Value << (int)settings.RenderingApi; // TODO Enum as string
			out << Key << "AntiAliasing" << Value << settings.AntiAliasing;
			out << Key << "Headless" << Value << settings.Headless;

			out << EndMap;

			std::ofstream fout(filepath);
			if (!fout.is_open()) PIT_ENGINE_ERR(General, "file could not be created");
			fout << out.c_str();
			fout.close();
		}

		static void Deserialize(const String& filepath, EngineSettings& settings) {
			std::ifstream fin(filepath);
			if (!fin.is_open()) PIT_ENGINE_ERR(General, "file could not be opened");

			std::stringstream strStream;
			strStream << fin.rdbuf();
			fin.close();

			using namespace YAML;
			Node data = YAML::Load(strStream.str());

			settings.WindowName = data["WindowName"].as<String>();
			settings.WindowToolbar = data["WindowToolbar"].as<bool>();
			settings.VSync = data["VSync"].as<bool>();
			settings.UIDocking = data["UIDocking"].as<bool>();
			settings.RenderingApi = Cast<RenderingAPI>(data["RenderingApi"].as<int>());
			settings.AntiAliasing = data["AntiAliasing"].as<int>();
			settings.Headless = data["Headless"].as<bool>();
		}
	};
}