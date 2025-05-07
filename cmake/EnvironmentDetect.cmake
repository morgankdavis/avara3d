
if (UNIX AND APPLE)
	message("[MACOS]")
	set(MACOS TRUE)
	set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
endif()

if (UNIX AND NOT APPLE AND NOT MINGW)
	message("[LINUX]")
	set(LINUX TRUE)
endif()

if (WIN32)
	message("[WIN32]")
	set(WINDOWS TRUE)
	set(WIN32 TRUE)
endif()

if (WIN64)
	message("[WIN64]")
	set(WINDOWS TRUE)
	set(WIN64 TRUE)
endif()

if (RPI)
	message("[RPI]")
	set(RPI TRUE)
endif()

if (MACOS OR WINDOWS OR LINUX)
	message("[DESKTOP]")
	set(DESKTOP TRUE)
endif()
