//
// Created by mkd on 1/19/24.
//

#include "ae/BuildInfo.h"

#include "magic_enum.hpp"

#include "BuildInfoDefines.h"


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
	//AE_LOG_I("BUILD TIME: {}", AE_BUILD_TIME);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

int BuildInfo::number() const {
	return _number;
}

BuildInfo::Version BuildInfo::version() const {
	return _version;
}

BuildInfo::TYPE BuildInfo::type() const {
	return _type;
}

BuildInfo::ORIGIN BuildInfo::origin() const {
	return _origin;
}

