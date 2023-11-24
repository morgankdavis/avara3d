cmake_minimum_required(VERSION 3.14)

project(magic_enum
    VERSION "0.9.3"
    HOMEPAGE_URL "https://github.com/Neargye/magic_enum"
    DESCRIPTION "A library that provides static reflection for enums, work with any enum type without any macro or boilerplate code."
    LANGUAGES CXX
)
set(CPACK_PACKAGE_VENDOR "Daniil Goncharov")

include(GNUInstallDirs)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

option(MAGIC_ENUM_OPT_BUILD_EXAMPLES "Build magic_enum examples" ${IS_TOPLEVEL_PROJECT})
option(MAGIC_ENUM_OPT_BUILD_TESTS "Build and perform magic_enum tests" ${IS_TOPLEVEL_PROJECT})
option(MAGIC_ENUM_OPT_INSTALL "Generate and install magic_enum target" ${IS_TOPLEVEL_PROJECT})

set(INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(EXPORT_NAMESPACE "${PROJECT_NAME}::")

add_library(${PROJECT_NAME} INTERFACE)
add_library(${EXPORT_NAMESPACE}${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${INCLUDES}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

