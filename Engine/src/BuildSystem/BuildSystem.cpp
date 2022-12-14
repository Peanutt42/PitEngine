#include "pch.hpp"
#include "BuildSystem.hpp"

namespace Pit {
	void BuildReport::Print() {
		if (Successful) {
			PIT_ENGINE_INFO(General, "Build state: Successful");
		}
		else
			PIT_ENGINE_WARN(General, "Build state: Failed");
		PIT_ENGINE_INFO(General, "OS: {}", OperatingSystem::ToString(OS));
		PIT_ENGINE_INFO(General, "Config: {}", BuildConfigurationToString(Config));
		PIT_ENGINE_INFO(General, "Build took {} seconds", TotalBuildTimeSeconds);
	}


	BuildReport BuildSystem::BuildGame(OperatingSystem::Type os, BuildConfiguration config) {
		Timer totalBuildTimer;
		bool successful = true;

		String buildDir = FileSystem::GetSandboxDir() + "Builds/" + OperatingSystem::ToString(os) + "/" + BuildConfigurationToString(config);
		if (!std::filesystem::exists(buildDir)) std::filesystem::create_directories(buildDir);
		String assetDir = buildDir + "/Assets";
		if (!std::filesystem::exists(assetDir)) std::filesystem::create_directories(assetDir);

		String sceneDir = assetDir + "/Scenes";
		if (!std::filesystem::exists(sceneDir)) std::filesystem::create_directories(sceneDir);
		String shaderDir = assetDir + "/Shaders";
		if (!std::filesystem::exists(shaderDir)) std::filesystem::create_directories(shaderDir);
		String textureDir = assetDir + "/Textures";
		if (!std::filesystem::exists(textureDir)) std::filesystem::create_directories(textureDir);
		String modelDir = assetDir + "/Models";
		if (!std::filesystem::exists(modelDir)) std::filesystem::create_directories(modelDir);
		String audioDir = assetDir + "/Audio";
		if (!std::filesystem::exists(audioDir)) std::filesystem::create_directories(audioDir);

		BuildReport report{ .Successful = successful,
							.OS = os,
							.Config = config,
							.TotalBuildTimeSeconds = (int)totalBuildTimer.ElapsedSeconds() };

		return report;
	}
}