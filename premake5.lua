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

RootDir = "../"

group "Dependencies"
	include "vendor/glad"
	include "vendor/glm"
	include "vendor/imgui"
	include "vendor/ImGuizmo"
	include "vendor/stb_image"
	include "vendor/tinyobjloader"
	include "vendor/yaml"
group ""

group "Core"
	include "Engine"
	include "Engine-ScriptCore"
	include "Editor"
group ""

group "Other"
	include "Tests"
	include "Sandbox"
group ""