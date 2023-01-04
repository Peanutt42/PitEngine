workspace "Sandbox"
	architecture "x64"
	startproject "Sandbox"

	configurations 
	{
		"Debug",
		"Release"
	}

	flags 
	{
		"MultiProcessorCompile"
	}

RootDir = "../../"

project "Sandbox"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("Binaries")
	objdir ("Intermediate")

	files
	{
		"Source/**.cs",
		"Properties/**.cs"
	}

	links
	{
		"Engine-ScriptCore"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

group "PitEngine"
	include "../../Engine-ScriptCore"
group ""