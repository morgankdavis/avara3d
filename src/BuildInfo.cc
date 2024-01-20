//
// Created by mkd on 1/19/24.
//

#include "BuildInfo.h"
#include "_BuildInfo.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BuildInfo& BuildInfo::Info() {

	static BuildInfo instance;
	return instance;
}

BuildInfo::BuildInfo():
		_number(AE_BUILD_NUMBER),
		_version({AE_VERSION_MAJOR, AE_VERSION_MINOR, AE_VERSION_PATCH}),
		_type(AE_BUILD_TYPE),
		_origin(AE_BUILD_ORIGIN) {

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

