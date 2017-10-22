
add_subdirectory(src)


file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)


include_directories("/usr/include" "/usr/local/include" "include")


if(MACOS) #---------- MACOS -----------
	
	# system frameworks
	
	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_OPENGL OpenGL)
	find_library(FRAMEWORK_IOKIT IOKit)

	set(FRAMEWORKS ${FRAMEWORK_COCOA} ${FRAMEWORK_OPENGL} ${FRAMEWORK_IOKIT})
	
	# local libraries
	
	#find_library(LIB_GAINPUT NAME gainput PATHS "lib/macos")
	

	#link_directories("/usr/local/lib/" "/System/Library/Frameworks")

	# compiler flags
	
	#-pedantic
	set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ \
		-Wall -mmacosx-version-min=10.8 -arch x86_64 \
		-fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-c++11-long-long -Wself-assign -Wno-reorder")
	
	# silence "MACOSX_RPATH is not specified for the following targets:"
	set(CMAKE_MACOSX_RPATH 1)
	
elseif(LINUX) #---------- LINUX -----------
	
	# local libraries
	
	#find_library(LIB_GAINPUT NAME gainput PATHS "lib/linux")
	#find_library(LIB_GLEW NAME GLEW PATHS "lib/linux64")
	
	# compiler flags
	
	#-pedantic
	#-Wno-c++11-long-long
	set(CMAKE_CXX_FLAGS "-std=c++14 \
		-Wall -fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-reorder -Wfloat-equal")
endif()
	
	
# assimp

#add_subdirectory(extern/assimp-4.0.1)
	
	
# GLEW

#add_subdirectory(extern/glew-2.1.0/build/cmake)
	
	
# GLFW
	
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw-3.2.1)


# ZLIB

add_subdirectory(extern/zlib-1.2.11)


# ae

add_library(ae ${HEADERS} ${SOURCES})

target_link_libraries(ae stb manymouse glfw GLEW assimp z) # ${LIB_GAINPUT}  ${LIB_GLEW}

if(MACOS)
	target_link_libraries(ae freetype boost_container ${FRAMEWORKS})
elseif(LINUX)
	target_link_libraries(ae GL X11 Xxf86vm Xrandr pthread Xi dl)
endif()

