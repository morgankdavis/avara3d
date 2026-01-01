# external/v-hacd-4.1.0/CMakeLists.txt

add_library(vhacd STATIC
		src/a3d_vhacd_impl.cc
)
add_library(vhacd::vhacd ALIAS vhacd)

target_include_directories(vhacd PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

set_target_properties(vhacd PROPERTIES POSITION_INDEPENDENT_CODE ON)
