
# GLM

#include_directories(${CMAKE_SOURCE_DIR}/avara-engine/external/glm-0.9.9.9)


# GLFW

#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#add_subdirectory(${CMAKE_SOURCE_DIR}/avara-engine/external/glfw-3.2.1 bin/glfw)


# GLEW 

#include_directories(${CMAKE_SOURCE_DIR}/avara-engine/external/glew-2.1.0/include)
#add_subdirectory(${CMAKE_SOURCE_DIR}/avara-engine/external/glew-2.1.0/build/cmake bin/glew)






add_subdirectory(01-import)
add_subdirectory(02-scenegraph)
add_subdirectory(03-input-camera)
add_subdirectory(04-geoprimitives)
add_subdirectory(05-solarsystem)
