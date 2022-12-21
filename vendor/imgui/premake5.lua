project "imgui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"
	staticruntime "off"

	targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"include/**.h",
		"include/**.c",
		"include/**.cpp",
		"include/**.hpp"
	}

	includedirs {
		"include",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/vulkan/Include"
	}

	defines {

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