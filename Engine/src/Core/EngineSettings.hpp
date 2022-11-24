#pragma once

#include "Core/CoreInclude.hpp"
#include <yaml-cpp/yaml.h>

namespace Pit {
	enum class RenderingAPI {
		None,
		OpenGL
	};
	static inline String RenderingApiToString(RenderingAPI api) {
		switch (api) {
		default: return "";
		case RenderingAPI::None: return "None";
		case RenderingAPI::OpenGL: return "OpenGl";
		}
	}
	static inline RenderingAPI StringToRenderingApi(const String& string) {
		if (string == "None") return RenderingAPI::None;
		if (string == "OpenGl") return RenderingAPI::OpenGL;
		return RenderingAPI::None;
	}

	/// <summary>
	/// Settings for initializing the GameEngine.
	/// Some settings get chosen from the code,
	/// some are chosen from the config file(end user) like vsync...
	/// </summary>
	struct EngineSettings {
		Array<String> ConsoleArgs;
		String WindowName;
		bool WindowToolbar;
		bool VSync;
		bool UIDocking;
		RenderingAPI RenderingApi;
		int AntiAliasing;
		bool Headless;

	private:
		String m_ConfigFilepath;
	public:

		EngineSettings(const int argc, const char* argv[], const String& configFilepath,
					   const String& windowName, bool windowToolbar,
					   bool uiDocking, bool headless = false)
			: WindowName(windowName), WindowToolbar(windowToolbar),
			  VSync(false), UIDocking(uiDocking),
			  RenderingApi(RenderingAPI::OpenGL), AntiAliasing(0), Headless(headless), m_ConfigFilepath(configFilepath) {

			ConsoleArgs.assign(argv, argv + argc);
			for (const auto& arg : ConsoleArgs) {
				if (arg == "-headless") Headless = true;
			}

			if (configFilepath != "NULL")
				Deserialize(configFilepath);
		}

		void Serialize(const String& filepath = "default") {
			using namespace YAML;
			Emitter out;
			out << BeginMap;

			out << Comment("EngineSettings");
			out << Key << "VSync" << Value << VSync;
			out << Key << "RenderingApi" << Value << RenderingApiToString(RenderingApi);
			out << Key << "AntiAliasing" << Value << AntiAliasing;

			out << EndMap;

			std::ofstream fout(filepath == "default" ? m_ConfigFilepath : filepath);
			if (!fout.is_open()) PIT_ENGINE_ERR(General, "file could not be created");
			fout << out.c_str();
			fout.close();
		}

		void Deserialize(const String& filepath) {
			if (!std::filesystem::exists(filepath)) Serialize(filepath);
			
			std::ifstream fin(filepath);
			if (!fin.is_open()) PIT_ENGINE_ERR(General, "file could not be opened");

			std::stringstream strStream;
			strStream << fin.rdbuf();
			fin.close();

			using namespace YAML;
			Node data = YAML::Load(strStream.str());

			VSync = data["VSync"].as<bool>();
			RenderingApi = StringToRenderingApi(data["RenderingApi"].as<String>());
			AntiAliasing = data["AntiAliasing"].as<int>();
		}
	};
}