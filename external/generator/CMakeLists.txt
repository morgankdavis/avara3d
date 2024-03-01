#cmake_minimum_required(VERSION 2.6) # mkd: changed to avoid deprication warning
cmake_minimum_required(VERSION 3.8)
project(generator)

#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#
#include(GNUInstallDirs)

if(MSVC)
	add_definitions(/W3)
else()
	# mkd: SHUT UP
	add_definitions(-std=c++14)
	#add_definitions(-std=c++11 -pedantic -Wall -Wextra)
endif()

#set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/include)

#option(GENERATOR_USE_GLM "Use GLM instead of GML (experimental)" OFF)
# mkd: turn GLM on
#option(GENERATOR_USE_GLM "Use GLM instead of GML (experimental)" ON)
#if(GENERATOR_USE_GLM)
	
	# mkd: turn GLM on
	include_directories(${CMAKE_SOURCE_DIR}/avara3d/external/glm-0.9.9)
	message(STATUS ${CMAKE_SOURCE_DIR})
	
	message(STATUS "Using GLM instead of GML for math (note the similar names).  Support for this is experimental!")
	#add_definitions(-DGENERATOR_USE_GLM -D GLM_ENABLE_EXPERIMENTAL)
	add_definitions(-DGENERATOR_USE_GLM) # mkd: disabled -DGLM_ENABLE_EXPERIMENTAL
#endif()

#if(GENERATOR_USE_GLM)
	option(GENERATOR_SUFFIX_GLM "Suffix library name with 'glm' when using GLM (otherwise, does nothing)" OFF)
	set(GLM_INCLUDE_DIR "" CACHE FILEPATH "GLM include path")

	if(GENERATOR_SUFFIX_GLM AND GENERATOR_USE_GLM)
		set(SUFFIX "-glm")
	endif()
#else()
#	set(GML_INCLUDE_DIR "" CACHE FILEPATH "GML include path")
#endif()

#if(GML_INCLUDE_DIR AND NOT GENERATOR_USE_GLM)
#	include_directories(SYSTEM ${GML_INCLUDE_DIR})
#elseif(GLM_INCLUDE_DIR AND GENERATOR_USE_GLM)
	include_directories(SYSTEM ${GLM_INCLUDE_DIR})
#endif()

#set(TARGET_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}generator${SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX})

#message(STATUS "Target name is '${TARGET_NAME}'")

add_subdirectory(src)
#add_subdirectory(images)

#install(DIRECTORY include/generator DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
#install(FILES lib/${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})


# mkd: added

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

	target_compile_options(generator PUBLIC 
		"-Wno-macro-redefined")
	
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	
	target_compile_options(generator PUBLIC 
		"-Wno-builtin-macro-redefined")
	
endif()
	