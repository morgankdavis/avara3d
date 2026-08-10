
set(A3D_EXTERNAL_ROOT "${CMAKE_CURRENT_LIST_DIR}")

# build everything static.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

# bullet

set(USE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD OFF CACHE BOOL "" FORCE)
set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(INSTALL_CMAKE_FILES OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET3 OFF CACHE BOOL "" FORCE)
set(BUILD_OPENCL OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET3_OPENCL OFF CACHE BOOL "" FORCE)

add_subdirectory(bullet-3.17)

# bullet changes this. turn it back off.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

target_compile_definitions(BulletDynamics PUBLIC BT_THREADSAFE=1)
target_compile_definitions(BulletCollision PUBLIC BT_THREADSAFE=1)
target_compile_definitions(LinearMath PUBLIC BT_THREADSAFE=1)

a3d_external_virtual_include_dir(
		bullet
		"${A3D_EXTERNAL_ROOT}/bullet-3.17/src")

add_library(a3d_external_bullet INTERFACE)
add_library(a3d::bullet ALIAS a3d_external_bullet)

target_link_libraries(a3d_external_bullet INTERFACE
		Bullet3Common
		BulletCollision
		BulletDynamics
		BulletInverseDynamics
		LinearMath)

target_include_directories(a3d_external_bullet SYSTEM INTERFACE
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_VINCLUDE_ROOT}>"
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_VINCLUDE_ROOT}/bullet>")

### fastgltf ###

add_subdirectory(fastgltf-0.9.0)

add_library(a3d_external_fastgltf INTERFACE)
add_library(a3d::fastgltf ALIAS a3d_external_fastgltf)

target_link_libraries(a3d_external_fastgltf INTERFACE
		fastgltf::fastgltf)

if(EMSCRIPTEN)
	target_compile_options(fastgltf PRIVATE
			-Wno-unused-variable
	)
endif()

### generator ###

add_subdirectory(generator)

add_library(a3d_external_generator INTERFACE)
add_library(a3d::generator ALIAS a3d_external_generator)

target_link_libraries(a3d_external_generator INTERFACE generator)

target_include_directories(a3d_external_generator SYSTEM INTERFACE
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_ROOT}/generator/include>")

### gif-h ###

a3d_external_virtual_include_dir(
		gif-h
		"${A3D_EXTERNAL_ROOT}/gif-h")

add_library(a3d_external_gif_h INTERFACE)
add_library(a3d::gif_h ALIAS a3d_external_gif_h)

target_include_directories(a3d_external_gif_h SYSTEM INTERFACE
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_VINCLUDE_ROOT}>"
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_VINCLUDE_ROOT}/gif-h>")

### glad ###

add_subdirectory(glad-3.3)

add_library(a3d_external_glad INTERFACE)
add_library(a3d::glad ALIAS a3d_external_glad)

target_link_libraries(a3d_external_glad INTERFACE glad)

target_include_directories(a3d_external_glad SYSTEM INTERFACE
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_ROOT}/glad-3.3/include>")

### glfw ###

if (A3D_DESKTOP)

	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

	add_subdirectory(glfw-3.4.0)

	add_library(a3d_external_glfw INTERFACE)
	add_library(a3d::glfw ALIAS a3d_external_glfw)

	target_link_libraries(a3d_external_glfw INTERFACE glfw)

	target_include_directories(a3d_external_glfw SYSTEM INTERFACE
			"$<BUILD_INTERFACE:${A3D_EXTERNAL_ROOT}/glfw-3.4.0/include>")

endif()

### imgui ###

add_subdirectory(imgui-1.92.5)

a3d_external_virtual_include_dir(
		imgui
		"${A3D_EXTERNAL_ROOT}/imgui-1.92.5")

add_library(a3d_external_imgui INTERFACE)
add_library(a3d::imgui ALIAS a3d_external_imgui)

target_link_libraries(a3d_external_imgui INTERFACE imgui)

target_include_directories(a3d_external_imgui SYSTEM INTERFACE
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_VINCLUDE_ROOT}>"
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_VINCLUDE_ROOT}/imgui>"
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_VINCLUDE_ROOT}/imgui/backends>")

if(EMSCRIPTEN)
	target_compile_options(imgui PRIVATE
			-Wno-deprecated-pragma
	)
endif()

### magic_enum ###

add_library(a3d_external_magic_enum INTERFACE)
add_library(a3d::magic_enum ALIAS a3d_external_magic_enum)

target_include_directories(a3d_external_magic_enum SYSTEM INTERFACE
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_ROOT}/magic_enum-0.9.3/include>")

### stb ###

add_subdirectory(stb)

add_library(a3d_external_stb INTERFACE)
add_library(a3d::stb ALIAS a3d_external_stb)

target_link_libraries(a3d_external_stb INTERFACE stb)

### v-hacd ###

add_subdirectory(v-hacd-4.1.0)

add_library(a3d_external_vhacd INTERFACE)
add_library(a3d::vhacd ALIAS a3d_external_vhacd)

target_link_libraries(a3d_external_vhacd INTERFACE vhacd)
