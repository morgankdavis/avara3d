//
// Created by mkd on 1/19/24.
//

#ifndef AVARA3D_CONFIGURATION_H
#define AVARA3D_CONFIGURATION_H


#ifdef LINUX
#include <linux/limits.h> // PATH_MAX
#endif

#ifdef MACOS
#include <sys/syslimits.h> // PATH_MAX
#endif

#ifdef WINDOWS
//#include <windows.h> // PATH_MAX?
#define NOMINMAX
#include <stdlib.h> // _MAX_PATH
#define PATH_MAX _MAX_PATH
#endif


namespace a3d {

	constexpr size_t MAX_DYNAMIC_LIGHTS =	32;
	constexpr size_t MAX_PATH_LEN =			PATH_MAX;
}


#endif //AVARA3D_CONFIGURATION_H
