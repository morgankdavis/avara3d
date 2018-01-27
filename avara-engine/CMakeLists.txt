
add_subdirectory(src)

include_directories(external)

#include_directories(external/fontstash)

add_subdirectory(external/manymouse)
include_directories(external/manymouse)

add_subdirectory(external/meshoptimizer)
include_directories(external/meshoptimizer)

add_subdirectory(external/generator)
include_directories(external/generator/include)

include_directories(external/stb)

include_directories(external/gif-h)


file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)


add_library(ae ${HEADERS} ${SOURCES})


if(MACOS)
	target_link_libraries(ae generator manymouse meshoptimizer glfw glew_s assimp z boost_system boost_filesystem)
	target_link_libraries(ae ${FRAMEWORKS})
elseif(LINUX)
	target_link_libraries(ae generator manymouse meshoptimizer glfw glew_s assimp z boost_system boost_filesystem)
	target_link_libraries(ae GL X11 Xxf86vm Xrandr pthread Xi dl)
elseif(WINDOWS)
	target_link_libraries(ae generator manymouse meshoptimizer glfw glew_s assimp boost_system boost_filesystem)
	if(MINGW64)
		target_link_libraries(ae winpthreads)
	endif()
	target_link_libraries(ae ${OPENGL_LIBRARY})
endif()


set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)


if(MACOS)
	
	link_directories("/usr/local/lib/" "/System/Library/Frameworks")

	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_OPENGL OpenGL)
	find_library(FRAMEWORK_IOKIT IOKit)

	set(FRAMEWORKS ${FRAMEWORK_COCOA} ${FRAMEWORK_OPENGL} ${FRAMEWORK_IOKIT})

	set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ -arch x86_64")
	
	set(CMAKE_MACOSX_RPATH 1) # silence "MACOSX_RPATH is not specified [...]"
	
elseif(LINUX)

	include_directories("/usr/include" "/usr/local/include")
	
	set(CMAKE_CXX_FLAGS "-std=c++14")

elseif(WINDOWS)

	if(MINGW64)
		add_subdirectory(external/winpthreads)
		include_directories(external/winpthreads/include)
	endif()

	if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -lpthread")
	endif()

endif()


target_compile_options(ae PRIVATE 
	"-fmessage-length=0"
	"-UGLFW_CDECL"
	"-Wno-float-equal"
	"-Wno-c++11-long-long"
	"-Wno-self-assign"
	"-Wno-reorder"
	"-Wno-address-of-packed-member"
	"-Wno-unused-function")


