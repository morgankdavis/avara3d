# Install script for directory: C:/Users/m/avara-engine/avara-engine/external/boost-1.67.0

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/avara-engine")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/m/avara-engine/build-msys/avara-engine/external/boost-1.67.0/libs/atomic/cmake_install.cmake")
  include("C:/Users/m/avara-engine/build-msys/avara-engine/external/boost-1.67.0/libs/chrono/cmake_install.cmake")
  include("C:/Users/m/avara-engine/build-msys/avara-engine/external/boost-1.67.0/libs/date_time/cmake_install.cmake")
  include("C:/Users/m/avara-engine/build-msys/avara-engine/external/boost-1.67.0/libs/exception/cmake_install.cmake")
  include("C:/Users/m/avara-engine/build-msys/avara-engine/external/boost-1.67.0/libs/filesystem/cmake_install.cmake")
  include("C:/Users/m/avara-engine/build-msys/avara-engine/external/boost-1.67.0/libs/regex/cmake_install.cmake")
  include("C:/Users/m/avara-engine/build-msys/avara-engine/external/boost-1.67.0/libs/system/cmake_install.cmake")
  include("C:/Users/m/avara-engine/build-msys/avara-engine/external/boost-1.67.0/libs/test/cmake_install.cmake")
  include("C:/Users/m/avara-engine/build-msys/avara-engine/external/boost-1.67.0/libs/thread/cmake_install.cmake")
  include("C:/Users/m/avara-engine/build-msys/avara-engine/external/boost-1.67.0/libs/timer/cmake_install.cmake")

endif()

