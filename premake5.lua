workspace "PitEngine"
	architecture "x64"

	configurations {
		"Debug",
		"Release"
	}

	flags {
		"MultiProcessorCompile",
		"LinkTimeOptimization",
		"FatalCompileWarnings"
	}

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

include "Editor"
include "Engine"
include "Sandbox"
include "Tests"