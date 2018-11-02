
workspace "audio_mixer"
	configurations { "Debug", "Release" }
	platforms { "Win32","Linux" }
	location "build"

filter { "platforms:Win32" }
    system "windows"
    architecture "x32"
	defines { "_WIN32", "WIN32" ,"KLIB_EXPORTS"}
	
filter { "platforms:Linux" }
    system "linux"
    architecture "x64"	
	defines { "LINUX", "linux" ,"POSIX"}
	
	
filter "configurations:Debug"
	defines { "DEBUG" , "_DEBUG"}
	symbols "On"
	optimize "Debug"
	
filter "configurations:Release"
	defines { "NDEBUG" }
	symbols "Off"
	optimize "Speed"
	vectorextensions "SSE2"


project "test"
	kind "ConsoleApp"
	language "C++"

	files{
		"audio_mixer/*.h",
		"audio_mixer/*.cc",
		"audio/*.h",
		"audio/*.cc",
		"rtc_base/*.h",
		"rtc_base/*.cc",
		"mixer/*.h",
		"mixer/*.cpp",
		"main.cpp",
	}
	defines {"WEBRTC_WIN","NOMINMAX"}
	includedirs{
		"./","ffmpeg/",
	}

	
