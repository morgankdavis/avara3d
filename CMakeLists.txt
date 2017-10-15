
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(avara-engine)


if(UNIX AND APPLE)
    set(MACOS TRUE)
    if(CMAKE_GENERATOR STREQUAL "Xcode")
        set(XCODE TRUE)
    endif()
endif()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(WIN32)
    set(WINDOWS TRUE)
endif()

#if(MSVS OR MSYS OR MINGW)
#    set(Visual studio?)
#endif()


if(XCODE)
	add_definitions("-DAPPLE" "-DMACOS" "-DXCODE")
elseif(MACOS)
	#add_definitions("-DAPPLE" "-DMACOS")
elseif(LINUX)
	add_definitions("-DLINUX")
endif()


add_subdirectory(avara-engine)
add_subdirectory(tests)
