# build native_app_glue as a static lib
cmake_minimum_required(VERSION 3.4.1)

include(AndroidNdkModules)
android_ndk_import_module_native_app_glue()

file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.c*)

add_library(NDKHelper
  STATIC
    ${HEADERS} ${SOURCES}
)
set_target_properties(NDKHelper
  PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    INTERFACE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:NDKHelper,INCLUDE_DIRECTORIES>
)
target_include_directories(NDKHelper
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(NDKHelper
  PUBLIC
    native_app_glue
    GLESv3
    EGL
    log
    android
    atomic
)
