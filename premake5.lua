workspace "PitEngine"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	flags {
		"MultiProcessorCompile",
		"LinkTimeOptimization"
	}

	warnings "Extra" -- /W4

	startproject "Editor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/glad"
	include "vendor/glm"
	include "vendor/imgui"
	include "vendor/spdlog"
	include "vendor/stb_image"
	include "vendor/tinyobjloader"
	include "vendor/yaml"
group ""

include "Editor"
include "Engine"
include "Sandbox"
include "Tests"