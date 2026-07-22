# external/stb/CMakeLists.txt

add_library(stb STATIC)
add_library(stb::stb ALIAS stb)

set(STB_IMPL "${CMAKE_CURRENT_LIST_DIR}/src/a3d_stb_impl.cc")
if(NOT EXISTS "${STB_IMPL}")
	message(FATAL_ERROR "Missing stb implementation file: ${STB_IMPL}")
endif()

target_sources(stb PRIVATE "${STB_IMPL}")

target_include_directories(stb PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

set_target_properties(stb PROPERTIES POSITION_INDEPENDENT_CODE ON)
