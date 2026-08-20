//
//  BuildInfo.h
//  avara3d
//
//  Created by Morgan Davis on 1/19/24.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BUILDINFO_H
#define AVARA3D_BUILDINFO_H

#include <ctime>
#include <iomanip>
#include <string>

namespace a3d {

    /** @brief Exposes version and build metadata compiled into the A3D library. */
    class BuildInfo {

    public:
        // [Public Types]

        /** @brief Semantic version components for an A3D build. */
        struct Version { // semver.org
            int major; ///< Major version component.
            int minor; ///< Minor version component.
            int patch; ///< Patch version component.
        };

        /** @brief Build configuration used to compile the library. */
        enum class Type {
            Debug,          ///< Debug build.
            Release,        ///< Release build.
            RelWithDebInfo, ///< Release build with debug information.
            MinSizeRel,     ///< Release build optimized for minimum size.
            Unknown         ///< Unrecognized or unavailable build configuration.
        };

        /** @brief Origin of the build metadata. */
        enum class Origin {
            CI,   ///< Build produced by the configured CI environment.
            AdHoc ///< Build produced outside the configured CI environment.
        };

        // [Public Static Member Functions]

        /** @brief Returns the process-wide BuildInfo for the linked A3D library. */
        static const BuildInfo& Info();

        /** @brief Formats @p version as a dotted major.minor.patch string. */
        static std::string      VersionString(const Version& version);

        /** @brief Returns the canonical string for @p type. */
        static std::string      TypeString(Type type);

        /** @brief Returns the canonical string for @p origin. */
        static std::string      OriginString(Origin origin);

        // [Public Member Functions]

        /** @brief Returns the CI build number, or zero for an ad-hoc build. */
        unsigned                number() const;

        /** @brief Returns the semantic version compiled into the library. */
        const Version&          version() const;

        /** @brief Returns the build configuration. */
        Type                    type() const;

        /** @brief Returns whether the build metadata originated from CI or an ad-hoc build. */
        Origin                  origin() const;

        /**
         * @brief Returns the recorded build time as calendar components.
         *
         * CI builds use the CI pipeline creation timestamp. Ad-hoc builds currently
         * do not contain a meaningful build timestamp.
         */
        const std::tm&          time() const;

    private:
        // [Private Lifecycle Functions]

        BuildInfo();

        BuildInfo(const BuildInfo&)            = delete;
        BuildInfo& operator=(const BuildInfo&) = delete;

        BuildInfo(BuildInfo&&)            = delete;
        BuildInfo& operator=(BuildInfo&&) = delete;

        // [Private Member Variables]

        unsigned   _number;
        Version    _version;
        Type       _type;
        Origin     _origin;
        std::tm    _time;
    };

}

#endif //AVARA3D_BUILDINFO_H
