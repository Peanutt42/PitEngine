project "glm"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"
	staticruntime "off"
	
	warnings "Default" -- /W3?

	targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"include/**.h",
		"include/**.c",
		"include/**.cpp",
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
		optimize "off"
		symbols "Default"
		
	filter { "configurations:Release" }
		buildoptions "/MD"
		runtime "Release"
		optimize "Full"
		symbols "Default"