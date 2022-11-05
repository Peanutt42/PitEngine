project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

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
		"%{wks.location}/vendor/glad/include",
		"%{wks.location}/vendor/EnTT/include",
		"%{wks.location}/vendor/glm/include",
		"%{wks.location}/vendor/spdlog/include",
		"%{wks.location}/vendor/vulkan/include",
		"%{wks.location}/vendor/imgui/include",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/tinyobjloader/include",
		"%{wks.location}/vendor/stb_image/include",
		"%{wks.location}/vendor/optick/include",
		"%{wks.location}/Engine/src"
	}

	libdirs {
		"%{wks.location}/vendor/vulkan/Lib",
		"%{wks.location}/vendor/glfw/lib-vc2022",
		"%{wks.location}/vendor/optick/lib/x64/release"
	}

	defines {
		"PIT_EDITOR"
	}

	links {
		"glad",
		"glm",
		"imgui",
		"spdlog",
		"stb_image",
		"tinyobjloader",
		"opengl32.lib",
		"glfw3_mt.lib",
		"Dbghelp.lib",
		"OptickCore.lib",
		"Engine"
	}

	filter { "configurations:Debug" }
		buildoptions "/MTd"
		runtime "Debug"
		symbols "on"
		
	filter { "configurations:Release" }
		buildoptions "/MT"
		runtime "Release"
		optimize "on"
		symbols "on"

	filter { "configurations:Dist" }
		buildoptions "/MT"
		runtime "Release"
		optimize "on"

	filter { "configurations:Debug" }
		defines { "DEBUG" }
	filter { "configurations:Release" }
		defines { "RELEASE" }
	filter { "configurations:Dist" }
		defines { "DIST" }