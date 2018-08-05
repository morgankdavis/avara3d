
include_directories(external)

if (NOT ANDROID)
	# GLFW
	
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	add_subdirectory(external/glfw-3.2.1)
	if (WINDOWS) # donno
		include_directories(external/glfw-3.2.1/include)
	endif()
	
	# GLEW
	
	add_subdirectory(external/glew-2.1.0)
	include_directories(external/glew-2.1.0/include)
	
	# assimp
	
	set(ENABLE_BOOST_WORKAROUND ON CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
	if (WINDOWS)
		set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE) # needed for windows?
	endif()
	add_subdirectory(external/assimp-3.3.1)
	include_directories(external/assimp-3.3.1/include)
	
	# many mouse
	
	add_subdirectory(external/manymouse)
	include_directories(external/manymouse)
	
	# imgui

	include_directories(external/imgui-1.62/include)
	add_subdirectory(external/imgui-1.62)
	
endif() # NOT ANDROID

# mesh optimizer

add_subdirectory(external/meshoptimizer)
include_directories(external/meshoptimizer)

# generator

add_subdirectory(external/generator)
include_directories(external/generator/include)

include_directories(external/stb)

# gif-h

include_directories(external/gif-h)

# stb

include_directories(external/stb)

# fontstash

include_directories(external/fontstash)

# bullet

include_directories(external/bullet3-2.87/src)
add_subdirectory(external/bullet3-2.87)



file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.c*)

include_directories(include)

add_library(ae STATIC ${HEADERS} ${SOURCES})


target_link_libraries(ae Bullet3Common BulletCollision Bullet3Collision BulletDynamics Bullet3Dynamics BulletInverseDynamics Bullet3Geometry Bullet3OpenCL_clew LinearMath Bullet2FileLoader)
target_link_libraries(ae boost_atomic boost_chrono boost_date_time boost_exception boost_filesystem boost_regex boost_system boost_test boost_thread boost_timer)


if (MACOS)
	
	target_link_libraries(ae generator manymouse meshoptimizer glfw glew_s assimp imgui z)
	target_link_libraries(ae ${FRAMEWORKS})
	
elseif (LINUX)
	
	target_link_libraries(ae generator manymouse meshoptimizer glfw glew_s assimp imgui z boost_system boost_filesystem)
	target_link_libraries(ae GL X11 Xxf86vm Xrandr pthread Xi dl)
	
elseif (WINDOWS)

	target_link_libraries(ae generator manymouse meshoptimizer glfw glew_s assimp imgui boost_system boost_filesystem imgui bz2)
	
	# wtf?
	if(MSYS)
		target_link_libraries(ae winpthread)
	else() # if(MINGW64)
		target_link_libraries(ae winpthreads)
	endif()
	
	target_link_libraries(ae ${OPENGL_LIBRARY})
	
elseif (ANDROID)
	
	target_link_libraries(ae generator meshoptimizer z boost_system boost_filesystem android EGL GLESv3)
	
endif()


set(CMAKE_CXX_STANDARD 14)


if (MACOS)
	
	link_directories("/usr/local/lib/" "/System/Library/Frameworks")

	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_OPENGL OpenGL)
	find_library(FRAMEWORK_IOKIT IOKit)

	set(FRAMEWORKS ${FRAMEWORK_COCOA} ${FRAMEWORK_OPENGL} ${FRAMEWORK_IOKIT})

	set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ -arch x86_64")
	
	set(CMAKE_MACOSX_RPATH 1) # silence "MACOSX_RPATH is not specified [...]"
	
elseif (LINUX)

	include_directories("/usr/include" "/usr/local/include")
	
	set(CMAKE_CXX_FLAGS "-std=c++14")

elseif (WINDOWS)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
	
elseif (ANDROID)

	add_subdirectory(external/NDKHelper)
	
	# Export ANativeActivity_onCreate(),
	# Refer to: https://github.com/android-ndk/ndk/issues/381.
	set_target_properties(ae PROPERTIES  LINK_FLAGS "-u ANativeActivity_onCreate")

	target_link_libraries(ae NDKHelper)

endif()

# windows:
#cc1plus.exe: warning: unrecognized command line option '-Wno-address-of-packed-member'
#cc1plus.exe: warning: unrecognized command line option '-Wno-self-assign'
#cc1plus.exe: warning: unrecognized command line option '-Wno-c++11-long-long'
#cc1plus.exe: warning: unrecognized command line option '-Wno-address-of-packed-member'
#cc1plus.exe: warning: unrecognized command line option '-Wno-self-assign'
#cc1plus.exe: warning: unrecognized command line option '-Wno-c++11-long-long'

target_compile_options(ae PRIVATE 
	"-fmessage-length=0"
	"-UGLFW_CDECL"
	"-Wno-float-equal"
	"-Wno-c++11-long-long"
	"-Wno-self-assign"
	"-Wno-reorder"
	"-Wno-address-of-packed-member"
	"-Wno-unused-function")
		
		
		
if (NOT ANDROID)
		
	# check build type
	
	message("CMAKE_BUILD_TYPE")
	message(CMAKE_BUILD_TYPE)
	message(type ${CMAKE_BUILD_TYPE})

	if ((CMAKE_BUILD_TYPE STREQUAL "Release") OR (CMAKE_BUILD_TYPE STREQUAL "MinSizeRelease"))
		set(INSTALL_DIR ${PROJECT_BINARY_DIR}/packaged/release)
	else()
		set(INSTALL_DIR ${PROJECT_BINARY_DIR}/packaged/debug)
	endif()
	
	set(LIB_INSTALL_DIR ${INSTALL_DIR}/${PROJECT_NAME}/)

	install(TARGETS ae
	        DESTINATION ${LIB_INSTALL_DIR}/lib)
		
	# TODO: make "public headers"	
	install(FILES ${HEADERS}
	        DESTINATION ${LIB_INSTALL_DIR}/include)
			
			
	# file(GLOB SHADERS shaders/*)
	# install(FILES ${SHADERS}
	#         DESTINATION ${INSTALL_DIR}/${PROJECT_NAME}/shaders)
	#
	# file(GLOB IMAGES images/*)
	# install(FILES ${IMAGES}
	#         DESTINATION ${INSTALL_DIR}/${PROJECT_NAME}/images)
	#
	# file(GLOB FONTS fonts/*)
	# install(FILES ${FONTS}
	#         DESTINATION ${INSTALL_DIR}/${PROJECT_NAME}/fonts)
			
	set(DATA_INSTALL_DIR ${INSTALL_DIR}/tests/data/)
			
	install(DIRECTORY "../tests/data/"
	        DESTINATION ${DATA_INSTALL_DIR})
			
	file(GLOB SHADERS shaders/*)
	install(FILES ${SHADERS}
	        DESTINATION ${DATA_INSTALL_DIR}/shaders)
	
	file(GLOB IMAGES images/*)
	install(FILES ${IMAGES}
	        DESTINATION ${DATA_INSTALL_DIR}/images)
	
	file(GLOB FONTS fonts/*)
	install(FILES ${FONTS}
	        DESTINATION ${DATA_INSTALL_DIR}/fonts)
			
endif()
		
		
