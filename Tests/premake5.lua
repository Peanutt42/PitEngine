project "Tests"
	kind "ConsoleApp"
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
		"%{wks.location}/packages/Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn.1.8.1.6/build/native/include",
		"src"
	}

	libdirs {
		"%{wks.location}/packages/Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn.1.8.1.6/lib/native/v140/windesktop/msvcstl/static/rt-dyn/x64/%{cfg.buildcfg}"
	}

	defines {
		
	}

	links {
		"gtest.lib",
		"gtest_main.lib"
	}

	filter { "configurations:Debug" }
		buildoptions "/MDd"
		runtime "Debug"
		symbols "on"
		
	filter { "configurations:Release" }
		buildoptions "/MD"
		runtime "Release"
		optimize "on"