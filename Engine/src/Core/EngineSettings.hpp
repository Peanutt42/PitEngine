
#pragma once

#include "Core/CoreInclude.hpp"
#include "Serialization/YAMLSerializer.hpp"

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
		String Prefix;
		String WindowName;
		bool WindowToolbar;
		bool VSync;
		int MaxFps;
		RenderingAPI RenderingApi;
		int AntiAliasing;
		bool Headless;
		bool OneInstanceOnly;

	public:

		EngineSettings(const int argc, const char* argv[], const String& prefix,
					   const String& windowName, bool windowToolbar,
					   bool headless = false, bool oneInstanceOnly = false)
			: Prefix(prefix), WindowName(windowName), WindowToolbar(windowToolbar),
			  VSync(false), MaxFps(-1), RenderingApi(RenderingAPI::OpenGL), AntiAliasing(0), Headless(headless), OneInstanceOnly(oneInstanceOnly) {

			ConsoleArgs.assign(argv, argv + argc);
			for (const auto& arg : ConsoleArgs) {
				if (arg == "-headless") Headless = true;
			}
		}

		void Serialize(const std::filesystem::path& filepath) {
			YAML::Emitter out;
			out << YAML::BeginMap;

			out << YAML::Comment("EngineSettings");
			out << YAML::Key << "VSync" << YAML::Value << VSync;
			out << YAML::Key << "MaxFps" << YAML::Value << MaxFps;
			out << YAML::Key << "RenderingApi" << YAML::Value << RenderingApiToString(RenderingApi);
			out << YAML::Key << "AntiAliasing" << YAML::Value << AntiAliasing;

			out << YAML::EndMap;
			Serialization::SaveYamlToFile(filepath, out);
		}

		void Deserialize(const std::filesystem::path& filepath) {
			if (!std::filesystem::exists(filepath)) Serialize(filepath);

			try {
				YAML::Node in = YAML::LoadFile(filepath.string());

				VSync = in["VSync"].as<bool>();
				MaxFps = in["MaxFps"].as<int>();
				RenderingApi = StringToRenderingApi(in["RenderingApi"].as<String>());
				AntiAliasing = in["AntiAliasing"].as<int>();
			}
			catch ([[maybe_unused]] const YAML::BadFile& badFile) {
				PIT_ENGINE_FATAL(General, "Failed to load EngineSettings '{}', error: file isn't in yaml-format", filepath.string());
			}
			catch (const YAML::ParserException& parserError) {
				PIT_ENGINE_FATAL(General, "Failed to parse EngineSettings '{}', error: {}", filepath.string(), parserError.what());
			}
		}
	};
}