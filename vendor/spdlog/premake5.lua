project "spdlog"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"
	staticruntime "off"

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
		buildoptions "/MDd"
		runtime "Debug"
		symbols "on"
		
	filter { "configurations:Release" }
		buildoptions "/MD"
		runtime "Release"
		optimize "on"
		symbols "on"

	filter { "configurations:Dist" }
		buildoptions "/MD"
		runtime "Release"
		optimize "on"