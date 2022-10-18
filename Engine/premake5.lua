project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	nuget { "Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn:1.8.1.6" }

	files {
		"src/**.h",
		"src/**.c",
		"src/**.cpp",
		"src/**.hpp"
	}

	includedirs {
		"src",
		"%{wks.location}/vendor/EnTT/include",
		"%{wks.location}/vendor/glm/include",
		"%{wks.location}/vendor/spdlog/include",
		"%{wks.location}/vendor/vulkan/include",
		"%{wks.location}/vendor/imgui/include",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/tinyobjloader/include",
		"%{wks.location}/vendor/stb_image/include"
	}

	
	libdirs {
		"%{wks.location}/vendor/vulkan/Lib",
		"%{wks.location}/vendor/glfw/lib-vc2022"
	}

	links {
		"glm",
		"imgui",
		"spdlog",
		"stb_image",
		"tinyobjloader",
		"vulkan-1.lib",
		"glfw3_mt.lib"
	}

	defines {
		-- if compiled for editor use
		"PIT_EDITOR"
	}

	filter { "configurations:Debug" }
		buildoptions "/MTd"
		runtime "Debug"
		symbols "on"
		
	filter { "configurations:Release" }
		buildoptions "/MT"
		runtime "Release"
		optimize "on"

	filter { "system:windows" }
		defines {
			"PIT_WINDOWS"
		}
	filter { "system:linux" }
		defines {
			"PIT_LINUX"
		}
	filter { "system:macosx" }
		defines {
			"PIT_MACOS"
		}