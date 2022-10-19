workspace "PitEngine"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	flags {
		"MultiProcessorCompile"
	}

	startproject "Editor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/glm"
	include "vendor/imgui"
	include "vendor/spdlog"
	include "vendor/stb_image"
	include "vendor/tinyobjloader"
group ""

include "Editor"
include "Engine"
include "Sandbox"
include "Tests"