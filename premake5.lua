workspace "PitEngine"
	architecture "x64"

	configurations {
		"Debug",
		"Release"
	}

	flags {
		"MultiProcessorCompile",
		"FatalCompileWarnings"
	}

	editandcontinue "Off"
	
	warnings "Extra" -- /W4

	startproject "Editor"

outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Dependencies"
	include "vendor/glad"
	include "vendor/glm"
	include "vendor/imgui"
	include "vendor/stb_image"
	include "vendor/tinyobjloader"
	include "vendor/yaml"
group ""

group "Core"
	include "Engine"
	include "Engine-ScriptCore"
group ""

group "Tools"
	include "Editor"
	include "Tests"
group ""

group "Misc"
	include "Sandbox"
group ""