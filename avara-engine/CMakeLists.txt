
include_directories(external)

if (ANDROID)

	# NDKHelper

	add_subdirectory(external/NDKHelper)
	
else()

	# GLFW
	
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
	add_subdirectory(external/glfw-3.3)
	include_directories(external/glfw-3.3/include)
	
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
	include_directories(external/manymouse/include)
	
	# imgui

	add_subdirectory(external/imgui-1.62)
	include_directories(external/imgui-1.62/include)
	
endif() # NOT ANDROID


if (MINGW AND UBUNTU_CROSS)

	# bzip2

	add_subdirectory(external/bzip2)
	include_directories(external/bzip2/include)

endif() # MINGW


# mesh optimizer

add_subdirectory(external/meshoptimizer)
include_directories(external/meshoptimizer/include)

# generator

add_subdirectory(external/generator)
include_directories(external/generator/include)

# gif-h

include_directories(external/gif-h)

# stb

include_directories(external/stb)

# bullet

add_subdirectory(external/bullet3-2.87)
include_directories(external/bullet3-2.87/src)


file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.c*)

include_directories(include)

add_library(ae STATIC ${HEADERS} ${SOURCES})


target_link_libraries(ae
	generator
	meshoptimizer
	Bullet3Common BulletCollision
	Bullet3Collision
	BulletDynamics
	Bullet3Dynamics
	BulletInverseDynamics
	Bullet3Geometry
	Bullet3OpenCL_clew
	LinearMath
	Bullet2FileLoader
	boost_atomic
	boost_chrono
	boost_date_time
	boost_exception
	boost_filesystem
	boost_regex
	boost_system
	boost_test
	boost_thread
	boost_timer)
	
if (DESKTOP)
	target_link_libraries(ae
		manymouse
		glfw
		glew_s
		assimp)
endif()

if (MACOS)
	
	target_link_libraries(ae imgui z)

	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_OPENGL OpenGL)
	find_library(FRAMEWORK_IOKIT IOKit)
	find_library(FRAMEWORK_CORE_VIDEO CoreVideo)
	set(FRAMEWORKS ${FRAMEWORK_COCOA} ${FRAMEWORK_OPENGL} ${FRAMEWORK_IOKIT} ${FRAMEWORK_CORE_VIDEO})
	target_link_libraries(ae ${FRAMEWORKS})

	set(CMAKE_MACOSX_RPATH 1) # silence "MACOSX_RPATH is not specified [...]"

elseif (LINUX)
	
	target_link_libraries(ae
		imgui
		z
		GL
		X11
		Xxf86vm
		Xrandr
		pthread
		Xi
		dl)
	
elseif (WINDOWS)

	target_link_libraries(ae
		imgui
		bz2)
	
	# wtf?
	if(MSYS OR UBUNTU_CROSS)
		target_link_libraries(ae winpthread)
	else()
		target_link_libraries(ae winpthreads)
	endif()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
	
elseif (ANDROID)
	
	target_link_libraries(ae
		android
		NDKHelper
		z
		EGL
		GLESv3)
		
	# Export ANativeActivity_onCreate(). Refer to: https://github.com/android-ndk/ndk/issues/381.
	set_target_properties(ae PROPERTIES LINK_FLAGS "-u ANativeActivity_onCreate")
	
endif()


# silence stupid warnings

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

	target_compile_options(ae PUBLIC
			"-fmessage-length=0"
			"-UGLFW_CDECL"
			"-Wno-unknown-warning-option"
			"-Wno-float-equal"
			"-Wno-c++11-long-long"
			"-Wno-self-assign"
			"-Wno-reorder"
			"-Wno-address-of-packed-member"
			"-Wno-unused-function"
			"-Wno-deprecated-declarations")
			#"-Wno-macro-redefined")
			
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")

# seems as if these are not needed for some reason... (?)

endif()
		
		
if (NOT ANDROID)
		
	# install lib and headers

	set(LIB_INSTALL_DIR "${ROOT_INSTALL_DIR}/${INSTALL_DIR}/${PROJECT_NAME}")

	install(TARGETS ae
	        DESTINATION "${LIB_INSTALL_DIR}/lib")
		
	# TODO: make "public headers"	
	install(FILES ${HEADERS}
	        DESTINATION "${LIB_INSTALL_DIR}/include")
	        
	# install engine resources

	file(GLOB SHADERS "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/shaders/*")
	install(FILES ${SHADERS}
			DESTINATION "${LIB_INSTALL_DIR}/shaders")

	file(GLOB IMAGES "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/images/*")
	install(FILES ${IMAGES}
			DESTINATION "${LIB_INSTALL_DIR}/images")

	file(GLOB FONTS "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/fonts/*")
	install(FILES ${FONTS}
			DESTINATION "${LIB_INSTALL_DIR}/fonts")
			
endif()
