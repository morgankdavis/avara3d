
find_package(OpenGL REQUIRED)

add_subdirectory(src)

add_subdirectory(external/generator)
include_directories(external/generator/include)


file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)


set(CMAKE_CXX_STANDARD 14)


if(MACOS)

	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_OPENGL OpenGL)
	find_library(FRAMEWORK_IOKIT IOKit)

	set(FRAMEWORKS ${FRAMEWORK_COCOA} ${FRAMEWORK_OPENGL} ${FRAMEWORK_IOKIT})
	
	# -stdlib=libc++ is needed in macOS…
	set(CMAKE_CXX_FLAGS "-stdlib=libc++ \
		-Wall -mmacosx-version-min=10.8 -arch x86_64 \
		-fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-c++11-long-long -Wself-assign -Wno-reorder")
	
	set(CMAKE_MACOSX_RPATH 1) # silence "MACOSX_RPATH is not specified [...]"
	
elseif(LINUX)

	set(CMAKE_CXX_FLAGS "-Wall -fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-reorder -Wfloat-equal")	

elseif(WINDOWS)

endif()
	

add_library(ae ${HEADERS} ${SOURCES})


if(MACOS)
	target_link_libraries(ae generator manymouse meshoptimizer stb glfw glew_s assimp z)
	target_link_libraries(ae ${FRAMEWORKS})
elseif(LINUX)
	target_link_libraries(ae generator manymouse meshoptimizer stb glfw glew_s assimp z)
	target_link_libraries(ae GL X11 Xxf86vm Xrandr pthread Xi dl)
elseif(WINDOWS)
	target_link_libraries(ae generator manymouse meshoptimizer stb glfw glew_s assimp)
	target_link_libraries(ae ${OPENGL_LIBRARY})
endif()

