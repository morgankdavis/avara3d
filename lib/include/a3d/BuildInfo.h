//
//  BuildInfo.h
//  avara3d
//
//  Created by Morgan Davis on 1/19/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BUILDINFO_H
#define AVARA3D_BUILDINFO_H

#include <ctime>
#include <iomanip>
#include <string>

namespace a3d {

	class BuildInfo {

	public:
		/// Public Types ///

		struct Version { // semver.org
			int major;
			int minor;
			int patch;
		};

		enum class Type {
			Debug,
			Release,
			RelWithDebInfo,
			MinSizeRel,
			Unknown
		};

		enum class Origin {
			CI,
			AdHoc
		};

		/// Public Static Member Functions ///

		static const BuildInfo&		Info();
		static std::string			VersionString(const Version& version);
		static std::string			TypeString(Type type);
		static std::string			OriginString(Origin origin);

		/// Public Member Functions ///

		unsigned 					number() const;
		const Version&				version() const;
		Type 						type() const;
		Origin 						origin() const;
		const std::tm&				time() const;

	private:
		/// Private Lifecycle Functions ///

		BuildInfo();

		BuildInfo(const BuildInfo&) = delete;
		BuildInfo& operator=(const BuildInfo&) = delete;

		BuildInfo(BuildInfo&&) = delete;
		BuildInfo& operator=(BuildInfo&&) = delete;

		/// Private Member Variables ///

		unsigned					_number;
		Version 					_version;
		Type	 					_type;
		Origin 						_origin;
		std::tm						_time;
	};
}

#endif //AVARA3D_BUILDINFO_H
