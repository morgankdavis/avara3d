//
//  BuildInfo.h
//  avara3d
//
//  Created by Morgan Davis on 1/19/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BUILDINFO_H
#define AVARA3D_BUILDINFO_H

#include <iomanip>

namespace a3d {

	class BuildInfo {

/*********************************************************************************************
	Public Types
 *********************************************************************************************/

	public:

		struct Version { // semver.org
			int major;
			int minor;
			int patch;
		};

		enum class Type {
			Debug,
			Release,
			RelWithDebInfo,
			MinSizeRel
		};

		enum class Origin {
			CI,
			AdHoc
		};

/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

		static BuildInfo& Info();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		unsigned 		number() const;
		const Version&	version() const;
		Type 			type() const;
		Origin 			origin() const;
		const std::tm&	time() const;

/*********************************************************************************************
	Private Lifecycle Functions
 *********************************************************************************************/

	private:

		BuildInfo();

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

		unsigned			_number;
		Version 			_version;
		Type	 			_type;
		Origin 				_origin;
		std::tm				_time;
	};
}

#endif //AVARA3D_BUILDINFO_H
