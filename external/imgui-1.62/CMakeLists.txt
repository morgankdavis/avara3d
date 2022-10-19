
cmake_minimum_required(VERSION 3.0)

project(imgui)


file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.c*)

include_directories(include)


# is there a better way to do this..?
#if (NOT ANDROID)
#	include_directories(../glfw-3.2.1/include)
#	include_directories(glew-2.1.0/include)
#endif()

if (NOT ANDROID)
	include_directories(../glfw-3.3b-mojave-fix/include)
	include_directories(glew-2.1.0/include)
endif()

#include_directories(${CMAKE_SOURCE_DIR}/avara-engine/include)

#add_subdirectory(glew-2.1.0/build/cmake)


add_library(imgui ${HEADERS} ${SOURCES})

if (NOT ANDROID)
	target_link_libraries(imgui glew_s) # gross
else()
	
	# link to any of these?
	
#	target_link_libraries(NDKHelper
# 	  PUBLIC
# 	    native_app_glue
# 	    GLESv3
# 	    EGL
# 	    log
# 	    android
# 	    atomic
# 	)
	
endif()
