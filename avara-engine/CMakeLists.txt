
add_subdirectory(src)

include_directories(external)

#include_directories(external/fontstash)

if (NOT ANDROID)
	add_subdirectory(external/manymouse)
	include_directories(external/manymouse)
endif()

add_subdirectory(external/meshoptimizer)
include_directories(external/meshoptimizer)

add_subdirectory(external/generator)
include_directories(external/generator/include)

include_directories(external/stb)

include_directories(external/gif-h)

# add_subdirectory(external/bullet3-2.87)
# include_directories(external/bullet3-2.87/src)


file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)


if (DESKTOP)
	add_library(ae STATIC ${HEADERS} ${SOURCES})
else()
	add_library(ae SHARED ${HEADERS} ${SOURCES})
endif()


target_link_libraries(ae Bullet3Common BulletCollision Bullet3Collision BulletDynamics Bullet3Dynamics BulletInverseDynamics Bullet3Geometry Bullet3OpenCL_clew LinearMath Bullet2FileLoader)

if (MACOS)
	target_link_libraries(ae generator manymouse meshoptimizer glfw glew_s assimp z boost_system boost_filesystem)
	target_link_libraries(ae ${FRAMEWORKS})
elseif (LINUX)
	target_link_libraries(ae generator manymouse meshoptimizer glfw glew_s assimp z boost_system boost_filesystem)
	target_link_libraries(ae GL X11 Xxf86vm Xrandr pthread Xi dl)
elseif (WINDOWS)
	target_link_libraries(ae generator manymouse meshoptimizer glfw glew_s assimp boost_system boost_filesystem)
	if(MINGW64)
		target_link_libraries(ae winpthreads)
	endif()
	target_link_libraries(ae ${OPENGL_LIBRARY})
elseif (ANDROID)
	# manymouse glfw glew_s
	#target_link_libraries(ae generator meshoptimizer assimp z boost_system boost_filesystem -landroid -lEGL -lGLESv3)
	# removed assimp
	target_link_libraries(ae generator meshoptimizer z boost_system boost_filesystem -landroid -lEGL -lGLESv3)
endif()


set(CMAKE_CXX_STANDARD 14)

#find_package(OpenGL REQUIRED)


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

	if(MINGW64)
		add_subdirectory(external/winpthreads)
		include_directories(external/winpthreads/include)
	endif()

	#if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -lpthread")
	#else()
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")
	#endif()
	
elseif (ANDROID)

	# things
	
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -android -native_app_glue")
	
	# target_link_libraries(ae
	#   PUBLIC
	#     native_app_glue
	#     android
	# )
	
	#target_link_libraries(ae NDKHelper)
	
	#target_link_libraries(ae -landroid android)
	
	#target_link_libraries(TeapotNativeActivity NDKHelper ae android)
	#target_link_libraries(ae android NDKHelper TeapotNativeActivity)
	
	# target_link_libraries(ae
	#     native_app_glue
	#     GLESv3
	#     EGL
	#     log
	#     android
	#     atomic
	# )
	
	add_subdirectory(external/NDKHelper)
	
	# Export ANativeActivity_onCreate(),
	# Refer to: https://github.com/android-ndk/ndk/issues/381.
	# set_target_properties(${PROJECT_NAME}
	set_target_properties(ae
	  PROPERTIES  LINK_FLAGS "-u ANativeActivity_onCreate")

	# add lib dependencies
	#target_link_libraries(${PROJECT_NAME}
	target_link_libraries(ae
	    NDKHelper
	)
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	

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
