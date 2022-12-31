
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
		bool UIDocking;
		RenderingAPI RenderingApi;
		int AntiAliasing;
		bool Headless;
		bool OneInstanceOnly;

	public:

		EngineSettings(const int argc, const char* argv[], const String& prefix,
					   const String& windowName, bool windowToolbar,
					   bool uiDocking, bool headless = false, bool oneInstanceOnly = false)
			: Prefix(prefix), WindowName(windowName), WindowToolbar(windowToolbar),
			  VSync(false), MaxFps(-1), UIDocking(uiDocking),
			  RenderingApi(RenderingAPI::OpenGL), AntiAliasing(0), Headless(headless), OneInstanceOnly(oneInstanceOnly) {

			ConsoleArgs.assign(argv, argv + argc);
			for (const auto& arg : ConsoleArgs) {
				if (arg == "-headless") Headless = true;
			}
		}

		void Serialize(const String& filepath) {
			Serialization::YamlSerializer out;
			out << YAML::BeginMap;

			out << YAML::Comment("EngineSettings");
			out << YAML::Key << "VSync" << YAML::Value << VSync;
			out << YAML::Key << "MaxFps" << YAML::Value << MaxFps;
			out << YAML::Key << "RenderingApi" << YAML::Value << RenderingApiToString(RenderingApi);
			out << YAML::Key << "AntiAliasing" << YAML::Value << AntiAliasing;

			out << YAML::EndMap;
			out.SaveToFile(filepath);
		}

		void Deserialize(const String& filepath) {
			if (!std::filesystem::exists(filepath)) Serialize(filepath);

			Serialization::YamlDeserializer in(filepath);

			in.Find("VSync", VSync);
			in.Find("MaxFps", MaxFps);
			String renderingApiStr;
			in.Find("RenderingApi", renderingApiStr);
			RenderingApi = StringToRenderingApi(renderingApiStr);
			in.Find("AntiAliasing", AntiAliasing);
		}
	};
}