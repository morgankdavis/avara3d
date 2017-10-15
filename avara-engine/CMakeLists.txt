
#add_subdirectory(include)
add_subdirectory(src)


file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)


include_directories("/usr/include" "/usr/local/include")


if(MACOS)
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)
	FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
	FIND_LIBRARY(IOKIT_LIBRARY IOKit)

	SET(FRAMEWORKS ${COCOA_LIBRARY} ${OPENGL_LIBRARY} ${IOKIT_LIBRARY})

	link_directories("/usr/local/lib/" "/System/Library/Frameworks")

	#-pedantic
	set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ \
		-Wall -mmacosx-version-min=10.8 -arch x86_64 \
		-fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-c++11-long-long -Wself-assign -Wno-reorder")
elseif(LINUX)
	#-pedantic
	#-Wno-c++11-long-long
	set(CMAKE_CXX_FLAGS "-std=c++14 \
		-Wall -fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-reorder -Wfloat-equal")
endif()


add_library(ae ${HEADERS} ${SOURCES})

target_link_libraries(ae stb manymouse)


if(MACOS)
	target_link_libraries(ae z glfw GLEW assimp freetype boost_container ${FRAMEWORKS})
elseif(LINUX)
	target_link_libraries(ae z GL X11 Xxf86vm Xrandr pthread Xi glfw GLEW assimp nanogui nanovg)
endif()

