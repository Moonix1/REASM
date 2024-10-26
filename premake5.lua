workspace "REASM"
	architecture "x86_64"
	configurations { "Debug", "Release" }

project "REASM"
	kind "ConsoleApp"
	language "C++"
    cppdialect "C++20"
	targetdir "bin/%{cfg.buildcfg}"
	objdir "bin/obj/%{cfg.buildcfg}"

	files { "src/**.h", "src/**.cpp" }

	includedirs {
		"libs/spdlog/include",
	}

	filter "configurations:Debug"
        defines { "REASM_DEBUG" }
        symbols "On"
    filter "configurations:Release"
        defines { "REASM_RELEASE" }
        optimize "On"