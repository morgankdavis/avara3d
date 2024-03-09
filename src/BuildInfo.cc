//
// Created by mkd on 1/19/24.
//

#include "a3d/BuildInfo.h"

#include "magic_enum.hpp"

#include "BuildInfo.cmake.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BuildInfo& BuildInfo::Info() {

	static BuildInfo instance;
	return instance;
}

BuildInfo::BuildInfo() {

	_number = A3D_BUILD_NUMBER;
	_version = {A3D_VERSION_MAJOR, A3D_VERSION_MINOR, A3D_VERSION_PATCH};
	_type = *magic_enum::enum_cast<BuildInfo::TYPE>(A3D_BUILD_TYPE);
	_origin = *magic_enum::enum_cast<BuildInfo::ORIGIN>(A3D_BUILD_ORIGIN);

	// ISO 8601
	// https://stackoverflow.com/questions/26895428/how-do-i-parse-an-iso-8601-date-with-optional-milliseconds-to-a-struct-tm-in-c
	int y, M, d, h, m, s;
	sscanf(A3D_BUILD_TIME, "%d-%d-%dT%d:%d:%dZ", &y, &M, &d, &h, &m, &s);
	_time = {};
	_time.tm_year = y;
	_time.tm_mon = std::max(0, M - 1);
	_time.tm_mday = d;
	_time.tm_hour = h;
	_time.tm_min = m;
	_time.tm_sec = s;
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

int BuildInfo::number() const {
	return _number;
}

const BuildInfo::Version& BuildInfo::version() const {
	return _version;
}

BuildInfo::TYPE BuildInfo::type() const {
	return _type;
}

BuildInfo::ORIGIN BuildInfo::origin() const {
	return _origin;
}

const std::tm& BuildInfo::time() const {
	return _time;
}
