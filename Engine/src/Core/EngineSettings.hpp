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
		default:
		case RenderingAPI::None: break;
		case RenderingAPI::OpenGL: return "OpenGl";
		}
	}
	static inline RenderingAPI StringToRenderingApi(const String& string) {
		if (string == "None") return RenderingAPI::None;
		if (string == "OpenGl") return RenderingAPI::OpenGL;
		return RenderingAPI::None;
	}

	struct EngineSettings {
		Array<String> ConsoleArgs;
		String WindowName;
		bool WindowToolbar;
		bool VSync;
		bool UIDocking;
		RenderingAPI RenderingApi;
		int AntiAliasing;
		bool Headless;

		EngineSettings(const int argc, const char* argv[], const String& configFilepath,
					   const String& windowName, bool windowToolbar,
					   bool uiDocking, bool headless = false)
			: WindowName(windowName), WindowToolbar(windowToolbar),
			  VSync(false), UIDocking(uiDocking),
			  RenderingApi(RenderingAPI::OpenGL), AntiAliasing(0), Headless(headless) {

			ConsoleArgs.assign(argv, argv + argc);
			for (const auto& arg : ConsoleArgs) {
				if (arg == "-headless") Headless = true;
			}

			if (configFilepath != "NullConfig.ini")
				Deserialize(configFilepath, *this);
		}

		static void Serialize(const String& filepath, EngineSettings& settings) {
			using namespace YAML;
			Emitter out;
			out << BeginMap;

			out << Comment("EngineSettings");
			out << Key << "VSync" << Value << settings.VSync;
			out << Key << "RenderingApi" << Value << RenderingApiToString(settings.RenderingApi);
			out << Key << "AntiAliasing" << Value << settings.AntiAliasing;

			out << EndMap;

			std::ofstream fout(filepath);
			if (!fout.is_open()) PIT_ENGINE_ERR(General, "file could not be created");
			fout << out.c_str();
			fout.close();
		}

		static void Deserialize(const String& filepath, EngineSettings& settings) {
			if (!std::filesystem::exists(filepath)) Serialize(filepath, settings);
			
			std::ifstream fin(filepath);
			if (!fin.is_open()) PIT_ENGINE_ERR(General, "file could not be opened");

			std::stringstream strStream;
			strStream << fin.rdbuf();
			fin.close();

			using namespace YAML;
			Node data = YAML::Load(strStream.str());

			settings.VSync = data["VSync"].as<bool>();
			settings.RenderingApi = StringToRenderingApi(data["RenderingApi"].as<String>());
			settings.AntiAliasing = data["AntiAliasing"].as<int>();
		}
	};
}