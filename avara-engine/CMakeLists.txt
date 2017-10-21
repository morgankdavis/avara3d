
add_subdirectory(src)


file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)


include_directories("/usr/include" "/usr/local/include" "include")


if(MACOS) #----- MACOS ------
	
	# system frameworks
	
	#rename these
	FIND_LIBRARY(FRAMEWORK_COCOA Cocoa)
	FIND_LIBRARY(FRAMEWORK_OPENGL OpenGL)
	FIND_LIBRARY(FRAMEWORK_IOKIT IOKit)

	SET(FRAMEWORKS ${FRAMEWORK_COCOA} ${FRAMEWORK_OPENGL} ${FRAMEWORK_IOKIT})
	
	# local libraries
	
	FIND_LIBRARY(LIB_GAINPUT NAME gainput PATHS "lib/macos")
	

	#link_directories("/usr/local/lib/" "/System/Library/Frameworks")

	# compiler flags
	
	#-pedantic
	set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ \
		-Wall -mmacosx-version-min=10.8 -arch x86_64 \
		-fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-c++11-long-long -Wself-assign -Wno-reorder")
	
elseif(LINUX) #----- LINUX ------
	
	# local libraries
	
	FIND_LIBRARY(LIB_GAINPUT NAME gainput PATHS "lib/linux")
	
	# compiler flags
	
	#-pedantic
	#-Wno-c++11-long-long
	set(CMAKE_CXX_FLAGS "-std=c++14 \
		-Wall -fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-reorder -Wfloat-equal")
endif()


add_library(ae ${HEADERS} ${SOURCES})

target_link_libraries(ae stb manymouse ${LIB_GAINPUT})


if(MACOS)
	target_link_libraries(ae z glfw GLEW assimp freetype boost_container ${FRAMEWORKS})
elseif(LINUX)
	target_link_libraries(ae z GL X11 Xxf86vm Xrandr pthread Xi dl glfw GLEW assimp)
endif()

