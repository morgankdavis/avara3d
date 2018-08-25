
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") # -Werror -Wunused-variable

include_directories(${PROJECT_SOURCE_DIR}/avara-engine/include)
include_directories(${PROJECT_SOURCE_DIR}/avara-engine/external/NDKHelper ABSOLUTE)

add_library(test_main
  SHARED
    test_main.cpp
)

set_target_properties(test_main
  PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set_target_properties(test_main
  PROPERTIES  LINK_FLAGS "-u ANativeActivity_onCreate")

target_link_libraries(test_main
	ae
)

