
find_package(OpenGL REQUIRED)


add_subdirectory(external)
add_subdirectory(src)

include_directories(external/generator/include)
#include_directories(external/glm-0.9.9.9)


file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)


if(MACOS) #---------- MACOS -----------
	
	include_directories("/usr/include" "/usr/local/include" "include")

	# system frameworks
	
	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_OPENGL OpenGL)
	find_library(FRAMEWORK_IOKIT IOKit)

	set(FRAMEWORKS ${FRAMEWORK_COCOA} ${FRAMEWORK_OPENGL} ${FRAMEWORK_IOKIT})
	
	# local libraries
	

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
	
	include_directories("/usr/include" "/usr/local/include" "include")

	# local libraries
	

	# compiler flags
	
	#-pedantic
	#-Wno-c++11-long-long
	set(CMAKE_CXX_FLAGS "-std=c++14 \
		-Wall -fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-reorder -Wfloat-equal")
		

elseif(WINDOWS) #---------- WINDOWS-----------

	#add_subdirectory(extern)
	
	# On Visual 2005 and above, this module can set the debug working directory
	#cmake_policy(SET CMP0026 OLD)
	#list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/extern/rpavlik-cmake-modules-fe2273")
	#include(CreateLaunchers)
	#include(MSVCMultipleProcessCompile) # /MP


	#set(ALL_LIBS
	#	${OPENGL_LIBRARY}
	#	glfw
	#	GLEW_1130
	#)

	#add_definitions(
	#	-DTW_STATIC
	#	-DTW_NO_LIB_PRAGMA
	#	-DTW_NO_DIRECT3D
	#	-DGLEW_STATIC
	#	-D_CRT_SECURE_NO_WARNINGS
	#)

endif()
	


# ae

add_library(ae ${HEADERS} ${SOURCES})



if(MACOS)
	target_link_libraries(ae generator manymouse meshoptimizer stb glfw glew assimp z)
	target_link_libraries(ae ${FRAMEWORKS})
elseif(LINUX)
	target_link_libraries(ae generator manymouse meshoptimizer stb glfw glew assimp z)
	target_link_libraries(ae GL X11 Xxf86vm Xrandr pthread Xi dl)
elseif(WINDOWS)
	target_link_libraries(ae generator manymouse meshoptimizer stb glfw glew_s assimp)
	target_link_libraries(ae ${OPENGL_LIBRARY})
endif()

