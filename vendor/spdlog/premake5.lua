project "spdlog"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"
	staticruntime "on"

	targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"include/**.h",
		"src/**.c",
		"src/**.cpp",
		"include/**.hpp"
	}

	includedirs {
		"include"
	}

	defines {

	}

	filter { "configurations:Debug" }
		buildoptions "/MTd"
		runtime "Debug"
		symbols "on"
		
	filter { "configurations:Release" }
		buildoptions "/MT"
		runtime "Release"
		optimize "on"