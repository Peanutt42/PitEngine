project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	
	debugdir "$(SolutionDir)"

	targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.hpp"
    pchsource "src/pch.cpp"

	files {
		"src/**.h",
		"src/**.c",
		"src/**.cpp",
		"src/**.hpp"
	}

	includedirs {
		"src",
		"%{wks.location}/vendor/sndfile/include",
		"%{wks.location}/vendor/filewatch/include",
		"%{wks.location}/vendor/glad/include",
		"%{wks.location}/vendor/EnTT/include",
		"%{wks.location}/vendor/glm/include",
		"%{wks.location}/vendor/fmt/include",
		"%{wks.location}/vendor/mono/include",
		"%{wks.location}/vendor/imgui/include",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/tinyobjloader/include",
		"%{wks.location}/vendor/stb_image/include",
		"%{wks.location}/vendor/optick/include",
		"%{wks.location}/vendor/yaml/include"
	}

	
	libdirs {
		"%{wks.location}/vendor/mono/lib/%{cfg.buildcfg}",
		"%{wks.location}/vendor/sndfile/lib",
		"%{wks.location}/vendor/glfw/lib-vc2022",
		"%{wks.location}/vendor/optick/lib/x64/release"
	}

	defines {
	}

	filter { "configurations:Debug" }
		buildoptions "/MDd"
		runtime "Debug"
		optimize "off"
		symbols "Default"
		
	filter { "configurations:Release" }
		buildoptions "/MD"
		runtime "Release"
		optimize "Full"
		symbols "Default"

	filter { "system:windows" }
		defines { "PIT_WINDOWS" }
	filter { "system:linux" }
		defines { "PIT_LINUX" }
	filter { "system:macosx" }
		defines { "PIT_MACOS" }

	filter { "configurations:Debug" }
		defines { "DEBUG" }
	filter { "configurations:Release" }
		defines { "RELEASE" }