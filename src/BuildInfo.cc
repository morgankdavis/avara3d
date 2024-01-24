//
// Created by mkd on 1/19/24.
//

#include "ae/BuildInfo.h"

#include "magic_enum.hpp"

#include "BuildInfo.cmake.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BuildInfo& BuildInfo::Info() {

	static BuildInfo instance;
	return instance;
}

BuildInfo::BuildInfo() {

	_number = AE_BUILD_NUMBER;
	_version = {AE_VERSION_MAJOR, AE_VERSION_MINOR, AE_VERSION_PATCH};
	_type = *magic_enum::enum_cast<BuildInfo::TYPE>(AE_BUILD_TYPE);
	_origin = *magic_enum::enum_cast<BuildInfo::ORIGIN>(AE_BUILD_ORIGIN);

	// ISO 8601
	// https://stackoverflow.com/questions/26895428/how-do-i-parse-an-iso-8601-date-with-optional-milliseconds-to-a-struct-tm-in-c
	int y, M, d, h, m, s;
	sscanf(AE_BUILD_TIME, "%d-%d-%dT%d:%d:%dZ", &y, &M, &d, &h, &m, &s);
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
