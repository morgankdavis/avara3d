//
//  BuildInfo.cc
//  avara3d
//
//  Created by Morgan Davis on 1/19/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/BuildInfo.h"

#include <algorithm>
#include <cstdio>
#include <format>

#include <magic_enum/magic_enum.hpp>

#include "BuildInfo.cmake.h"

using namespace a3d;
using namespace std;

/// Public Static Member Functions ///

const BuildInfo& BuildInfo::Info() {

    static BuildInfo instance;
    return instance;
}

string BuildInfo::VersionString(const Version& version) {
    return std::format("{}.{}.{}", version.major, version.minor, version.patch);
}

string BuildInfo::TypeString(Type type) {
    switch (type) {
        case Type::Debug:
            return "Debug";
        case Type::Release:
            return "Release";
        case Type::RelWithDebInfo:
            return "RelWithDebInfo";
        case Type::MinSizeRel:
            return "MinSizeRel";
        case Type::Unknown:
            return "Unknown";
    }
}

string BuildInfo::OriginString(Origin origin) {
    switch (origin) {
        case Origin::CI:
            return "CI";
        case Origin::AdHoc:
            return "AdHoc";
    }
}

/// Public Member Functions ///

unsigned BuildInfo::number() const {
    return _number;
}

const BuildInfo::Version& BuildInfo::version() const {
    return _version;
}

BuildInfo::Type BuildInfo::type() const {
    return _type;
}

BuildInfo::Origin BuildInfo::origin() const {
    return _origin;
}

const std::tm& BuildInfo::time() const {
    return _time;
}

/// Private Lifecycle Functions ///

BuildInfo::BuildInfo() {

    _number = A3D_BUILD_NUMBER;
    _version = {A3D_VERSION_MAJOR, A3D_VERSION_MINOR, A3D_VERSION_PATCH};
    auto typeOpt = magic_enum::enum_cast<BuildInfo::Type>(A3D_BUILD_TYPE_STR);
    _type = typeOpt.value_or(BuildInfo::Type::Unknown);
    auto originOpt = magic_enum::enum_cast<BuildInfo::Origin>(A3D_BUILD_ORIGIN);
    _origin = originOpt.value_or(BuildInfo::Origin::AdHoc);

    // ISO 8601
    // https://stackoverflow.com/questions/26895428/how-do-i-parse-an-iso-8601-date-with-optional-milliseconds-to-a-struct-tm-in-c
    int y, M, d, h, m, s;
    sscanf(A3D_BUILD_TIME, "%d-%d-%dT%d:%d:%dZ", &y, &M, &d, &h, &m, &s);
    _time = {};
    _time.tm_year = y - 1900;
    _time.tm_mon = std::max(0, M - 1);
    _time.tm_mday = d;
    _time.tm_hour = h;
    _time.tm_min = m;
    _time.tm_sec = s;
}
