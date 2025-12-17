
if (UNIX AND APPLE)
	message("[MACOS]")
	set(A3D_MACOS TRUE)
	set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
endif()

if (UNIX AND NOT APPLE AND NOT MINGW)
	message("[LINUX]")
	set(A3D_LINUX TRUE)
endif()

if (WIN32)
	message("[WIN32]")
	set(A3D_WINDOWS TRUE)
	set(A3D_WIN32 TRUE)
endif()

if (WIN64)
	message("[WIN64]")
	set(A3D_WINDOWS TRUE)
	set(A3D_WIN64 TRUE)
endif()

if (RPI) # is this a command line option or something?
	message("[RPI]")
	set(A3D_RPI TRUE)
endif()

if (A3D_MACOS OR A3D_WINDOWS OR A3D_LINUX)
	message("[DESKTOP]")
	set(A3D_DESKTOP TRUE)
	set(A3D_GL_DESKTOP TRUE)
endif()
