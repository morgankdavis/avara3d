# Install script for directory: C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/m/avara-engine/build-msys/lib/libassimp.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/anim.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/ai_assert.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/camera.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/color4.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/color4.inl"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/config.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/defs.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/cfileio.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/light.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/material.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/material.inl"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/matrix3x3.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/matrix3x3.inl"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/matrix4x4.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/matrix4x4.inl"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/mesh.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/postprocess.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/quaternion.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/quaternion.inl"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/scene.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/metadata.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/texture.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/types.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/vector2.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/vector2.inl"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/vector3.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/vector3.inl"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/version.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/cimport.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/importerdesc.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/Importer.hpp"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/DefaultLogger.hpp"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/ProgressHandler.hpp"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/IOStream.hpp"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/IOSystem.hpp"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/Logger.hpp"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/LogStream.hpp"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/NullLogger.hpp"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/cexport.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/Exporter.hpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/Compiler/pushpack1.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/Compiler/poppack1.h"
    "C:/Users/m/avara-engine/avara-engine/external/assimp-3.3.1/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

