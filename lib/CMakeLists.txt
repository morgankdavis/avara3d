
### add external libraries ###

add_subdirectory(external)

### add a3d ###

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*")

add_library(avara3d STATIC
		${HEADERS}
		${SOURCES})

add_library(a3d::avara3d ALIAS avara3d)

target_compile_features(avara3d PUBLIC cxx_std_20)

### create BuildInfo.cmake.h ###

# compile-time string for both single- and multi-config
target_compile_definitions(avara3d
		PUBLIC
		$<$<CONFIG:Debug>:A3D_DEBUG>
		PRIVATE
		A3D_BUILD_TYPE_STR=\"$<CONFIG>\"
)
if (NOT DEFINED GITLAB_CI_PIPELINE_IID OR GITLAB_CI_PIPELINE_IID STREQUAL "")
	set(A3D_BUILD_NUMBER 0)
	set(A3D_BUILD_TIME "0000-00-00T00:00:00Z")
	set(A3D_BUILD_ORIGIN "AdHoc")
else ()
	set(A3D_BUILD_NUMBER ${GITLAB_CI_PIPELINE_IID})
	set(A3D_BUILD_TIME ${GITLAB_CI_PIPELINE_CREATED_AT})
	set(A3D_BUILD_ORIGIN "CI")
endif ()

configure_file(
		"${PROJECT_SOURCE_DIR}/cmake/BuildInfo.cmake.in"
		"${PROJECT_BINARY_DIR}/BuildInfo.cmake.h")

### setup a3d environment defines ###

a3d_set_compile_definitions(avara3d)

### add library headers ###

target_include_directories(avara3d
		PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
		PRIVATE
		"${PROJECT_BINARY_DIR}") # BuildInfo.cmake.h

target_link_libraries(avara3d
		PRIVATE
		a3d::bullet
		a3d::fastgltf
		a3d::generator
		a3d::imgui
		a3d::magic_enum
		a3d::stb
		a3d::vhacd
		a3d::webp)

### link a3d to platform-specific libraries ###

if (A3D_GL_DESKTOP)

	target_link_libraries(avara3d
			PUBLIC
			a3d::glad)

endif()

if (A3D_DESKTOP)

	target_link_libraries(avara3d
			PRIVATE
			a3d::glfw)

endif()

if (A3D_LINUX)

	find_package(Threads REQUIRED)
	target_link_libraries(avara3d
			PRIVATE
			Threads::Threads
			${CMAKE_DL_LIBS})

	if (A3D_RPI)

		find_library(EGL_LIBRARY EGL)
		find_library(OPENGLES3_LIBRARY GLESv2)

		target_link_libraries(avara3d
				PRIVATE
				${EGL_LIBRARY}
				${OPENGLES3_LIBRARY})

	else ()

		target_link_libraries(avara3d
				PRIVATE
				GL)

	endif ()

elseif (A3D_MACOS)

	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_CORE_VIDEO CoreVideo)
	find_library(FRAMEWORK_IOKIT IOKit)
	find_library(FRAMEWORK_OPENGL OpenGL)

	set(FRAMEWORKS
			${FRAMEWORK_COCOA}
			${FRAMEWORK_CORE_VIDEO}
			${FRAMEWORK_IOKIT}
			${FRAMEWORK_OPENGL})

	target_link_libraries(avara3d
			PRIVATE
			${FRAMEWORKS}
			z)

endif ()

### install ###

set(INSTALL_LIB_DIR "${INSTALL_ROOT_DIR}/lib")
set(INSTALL_DATA_DIR "${INSTALL_LIB_DIR}/data")
set(INSTALL_INCLUDE_DIR "${INSTALL_LIB_DIR}/include")
set(INSTALL_LEGAL_DIR "${INSTALL_ROOT_DIR}/legal")

if (NOT A3D_WEB)

	install(DIRECTORY
			"${CMAKE_CURRENT_SOURCE_DIR}/include/"
			DESTINATION "${INSTALL_INCLUDE_DIR}")

	install(DIRECTORY
			"${CMAKE_CURRENT_SOURCE_DIR}/data/"
			DESTINATION "${INSTALL_DATA_DIR}")

	install(DIRECTORY
			"${CMAKE_CURRENT_SOURCE_DIR}/external/generator"
			DESTINATION "${INSTALL_LEGAL_DIR}")

	install(TARGETS avara3d
			DESTINATION "${INSTALL_LIB_DIR}")

endif()
