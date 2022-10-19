
INCLUDE_DIRECTORIES(
	${BULLET_PHYSICS_SOURCE_DIR}/src
)

SET(Bullet3Common_SRCS
	b3AlignedAllocator.cpp
	b3Vector3.cpp
	b3Logging.cpp
)

SET(Bullet3Common_HDRS
	b3AlignedAllocator.h
	b3AlignedObjectArray.h
	b3CommandLineArgs.h
	b3HashMap.h
	b3Logging.h
	b3Matrix3x3.h
	b3MinMax.h
	b3PoolAllocator.h
	b3QuadWord.h
	b3Quaternion.h
	b3Random.h
	b3Scalar.h
	b3StackAlloc.h
	b3Transform.h
	b3TransformUtil.h
	b3Vector3.h
	shared/b3Float4.h  # mkd: added .h to avoid 'Policy CMP0115 is not set'
	shared/b3Int2.h
	shared/b3Int4.h
	shared/b3Mat3x3.h
	shared/b3PlatformDefinitions.h # mkd: added .h to avoid 'Policy CMP0115 is not set'
	shared/b3Quat.h
)

# switched to static by Morgan
ADD_LIBRARY(Bullet3Common STATIC ${Bullet3Common_SRCS} ${Bullet3Common_HDRS})
SET_TARGET_PROPERTIES(Bullet3Common PROPERTIES VERSION ${BULLET_VERSION})
SET_TARGET_PROPERTIES(Bullet3Common PROPERTIES SOVERSION ${BULLET_VERSION})

# added by Morgan
if(UNIX AND APPLE)
	target_compile_options(Bullet3Common PRIVATE
		"-mmacosx-version-min=10.9"
		"-Wno-argument-outside-range")
endif()

# disabled by Morgan
# IF (INSTALL_LIBS)
# 	IF (NOT INTERNAL_CREATE_DISTRIBUTABLE_MSVC_PROJECTFILES)
# 		#FILES_MATCHING requires CMake 2.6
# 		IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)
# 			IF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
# 				INSTALL(TARGETS Bullet3Common DESTINATION .)
# 			ELSE (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
# 				INSTALL(TARGETS Bullet3Common
# 					RUNTIME DESTINATION bin
# 					LIBRARY DESTINATION lib${LIB_SUFFIX}
# 					ARCHIVE DESTINATION lib${LIB_SUFFIX})
# 				INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
# DESTINATION ${INCLUDE_INSTALL_DIR} FILES_MATCHING PATTERN "*.h"  PATTERN
# ".svn" EXCLUDE PATTERN "CMakeFiles" EXCLUDE)
# 			ENDIF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
# 		ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)
#
# 		IF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
# 			SET_TARGET_PROPERTIES(Bullet3Common PROPERTIES FRAMEWORK true)
# 			SET_TARGET_PROPERTIES(Bullet3Common PROPERTIES PUBLIC_HEADER "${Bullet3Common_HDRS}")
# 		ENDIF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
# 	ENDIF (NOT INTERNAL_CREATE_DISTRIBUTABLE_MSVC_PROJECTFILES)
# ENDIF (INSTALL_LIBS)
