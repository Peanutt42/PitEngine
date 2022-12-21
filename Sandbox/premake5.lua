project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	
	debugdir "$(SolutionDir)"

	targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.hpp"
    pchsource "pch.cpp"

	files {
		"src/**.h",
		"src/**.c",
		"src/**.cpp",
		"src/**.hpp"
	}

	includedirs {
		"src",
		"%{wks.location}/vendor/sndfile/include",
		"%{wks.location}/vendor/glad/include",
		"%{wks.location}/vendor/EnTT/include",
		"%{wks.location}/vendor/glm/include",
		"%{wks.location}/vendor/spdlog/include",
		"%{wks.location}/vendor/vulkan/include",
		"%{wks.location}/vendor/imgui/include",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/tinyobjloader/include",
		"%{wks.location}/vendor/bullet/src",
		"%{wks.location}/vendor/stb_image/include",
		"%{wks.location}/vendor/optick/include",
		"%{wks.location}/vendor/yaml/include",
		"%{wks.location}/Engine/src"
	}

	libdirs {
		"%{wks.location}/vendor/vulkan/Lib",
		"%{wks.location}/vendor/sndfile/lib",
		"%{wks.location}/vendor/glfw/lib-vc2022",
		"%{wks.location}/vendor/optick/lib/x64/release"
	}

	defines {

	}

	links {
		"glm",
		"glad",
		"imgui",
		"spdlog",
		"stb_image",
		"tinyobjloader",
		"yaml",
		"sndfile.lib",
		"opengl32.lib",
		"glfw3.lib",
		"Dbghelp.lib",
		"OptickCore.lib",
		"Engine"
	}
	
	postbuildcommands {
		"%{wks.location}/Scripts/CopyDlls_" .. outputdir .. ".bat"
	}

	filter { "configurations:Debug" }
		buildoptions "/MDd"
		runtime "Debug"
		optimize "Debug"
		symbols "on"
		
	filter { "configurations:Release" }
		buildoptions "/MD"
		runtime "Release"
		optimize "on"
		symbols "on"

	filter { "configurations:Debug" }
		defines { "DEBUG" }
	filter { "configurations:Release" }
		defines { "RELEASE" }