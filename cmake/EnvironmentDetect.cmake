#
#if (UNIX AND APPLE)
#	message("[MACOS]")
#	set(A3D_MACOS TRUE)
#	set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
#endif()
#
#if (UNIX AND NOT APPLE AND NOT MINGW)
#	message("[LINUX]")
#	set(A3D_LINUX TRUE)
#endif()
#
#if (WIN32)
#	message("[WIN32]")
#	set(A3D_WINDOWS TRUE)
#	set(A3D_WIN32 TRUE)
#endif()
#
#if (WIN64)
#	message("[WIN64]")
#	set(A3D_WINDOWS TRUE)
#	set(A3D_WIN64 TRUE)
#endif()
#
#if (RPI) # is this a command line option or something?
#	message("[RPI]")
#	set(A3D_RPI TRUE)
#endif()
#
#if (A3D_MACOS OR A3D_WINDOWS OR A3D_LINUX)
#	message("[DESKTOP]")
#	set(A3D_DESKTOP TRUE)
#	set(A3D_GL_DESKTOP TRUE)
#endif()



# ---------- user knobs ----------
option(A3D_MACOS_UNIVERSAL "Build universal (x86_64 + arm64) on macOS" OFF)
option(A3D_RPI "Build for Raspberry Pi (EGL + OpenGL ES)" OFF)

# ---------- reset flags ----------
set(A3D_WINDOWS FALSE)
set(A3D_WIN32   FALSE)
set(A3D_WIN64   FALSE)

set(A3D_LINUX   FALSE)
set(A3D_MACOS   FALSE)
set(A3D_IOS     FALSE)
set(A3D_ANDROID FALSE)
set(A3D_EMSCRIPTEN FALSE)

set(A3D_DESKTOP FALSE)
set(A3D_MOBILE  FALSE)
set(A3D_WEB     FALSE)

set(A3D_POSIX   FALSE)

# GL backend family flags
set(A3D_GL_DESKTOP FALSE)  # OpenGL (desktop)
set(A3D_GL_ES      FALSE)  # OpenGL ES (mobile / pi)
set(A3D_GL_WEB     FALSE)  # WebGL (emscripten)

# ---------- toolchain / platform detection ----------

# Emscripten: usually defines EMSCRIPTEN and sets CMAKE_SYSTEM_NAME to "Emscripten"
if (DEFINED EMSCRIPTEN OR CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
	message("[EMSCRIPTEN]")
	set(A3D_EMSCRIPTEN TRUE)
	set(A3D_WEB TRUE)
	set(A3D_POSIX TRUE)          # emscripten behaves POSIX-ish enough for most uses
	set(A3D_GL_WEB TRUE)         # WebGL 2 ~= GLES 3
	#set(A3D_GL_ES TRUE)
endif()

# Android: CMake sets ANDROID when using the NDK toolchain
if (ANDROID)
	message("[ANDROID]")
	set(A3D_ANDROID TRUE)
	set(A3D_MOBILE TRUE)
	set(A3D_POSIX TRUE)
	set(A3D_GL_ES TRUE)
endif()

# Apple platforms
if (APPLE)
	# iOS is identified by CMAKE_SYSTEM_NAME = iOS (common with ios.toolchain.cmake / Xcode)
	# and/or CMAKE_OSX_SYSROOT containing "iphone"
	if (CMAKE_SYSTEM_NAME STREQUAL "iOS" OR
	(DEFINED CMAKE_OSX_SYSROOT AND CMAKE_OSX_SYSROOT MATCHES "iphone"))
		message("[iOS]")
		set(A3D_IOS TRUE)
		set(A3D_MOBILE TRUE)
		set(A3D_POSIX TRUE)
		set(A3D_GL_ES TRUE) # iOS uses OpenGL ES (or Metal; if you go Metal later, flip this)
	else()
		message("[MACOS]")
		set(A3D_MACOS TRUE)
		set(A3D_DESKTOP TRUE)
		set(A3D_POSIX TRUE)
		set(A3D_GL_DESKTOP TRUE)

		if (A3D_MACOS_UNIVERSAL)
			set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "" FORCE)
		endif()
	endif()
endif()

# Windows (covers MSVC, clang-cl, MinGW, etc.)
if (WIN32)
	message("[WINDOWS]")
	set(A3D_WINDOWS TRUE)
	set(A3D_DESKTOP TRUE)

	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(A3D_WIN64 TRUE)
		message("[WIN64]")
	else()
		set(A3D_WIN32 TRUE)
		message("[WIN32]")
	endif()
endif()

# Linux (native or cross, but not Android / Emscripten which are handled above)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT A3D_ANDROID AND NOT A3D_EMSCRIPTEN)
	message("[LINUX]")
	set(A3D_LINUX TRUE)
	set(A3D_DESKTOP TRUE)
	set(A3D_POSIX TRUE)

	# RPi is a specialization: desktop linux vs GLES+EGL path
	if (A3D_RPI)
		message("[RPI]")
		set(A3D_GL_ES TRUE)
	else()
		set(A3D_GL_DESKTOP TRUE)
	endif()
endif()

# ---------- sanity / summary ----------
if (A3D_DESKTOP)
	message("[DESKTOP]")
endif()
if (A3D_MOBILE)
	message("[MOBILE]")
endif()
if (A3D_WEB)
	message("[WEB]")
endif()
if (A3D_POSIX)
	message("[POSIX]")
endif()
