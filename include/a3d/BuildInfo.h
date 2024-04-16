//
//  BuildInfo.h
//	avara3d
//
//  Created by Morgan Davis on 1/19/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BUILDINFO_H
#define AVARA3D_BUILDINFO_H


#include <iomanip>


namespace a3d {

	class BuildInfo {

	public:

		/*********************************************************************************************
			Types
		 *********************************************************************************************/

		struct Version { // semver.org
			int major;
			int minor;
			int patch;
		};

		enum class Type {
			Debug,
			Release
		};

		enum class Origin {
			CI,
			AdHoc
		};

		/*********************************************************************************************
			Lifecycle
		 *********************************************************************************************/

		static BuildInfo& Info();

//		BuildInfo(const BuildInfo&) = delete; // copy constructor
//		BuildInfo &operator=(const BuildInfo&) = delete; // copy assignment
//		BuildInfo(BuildInfo&&) = delete; // move constructor
//		BuildInfo(const BuildInfo&&) = delete; // move assignment

		/*********************************************************************************************
			Public
		 *********************************************************************************************/

		int 			number() const;
		const Version&	version() const;
		Type 			type() const;
		Origin 			origin() const;
		const std::tm&	time() const;

		/*********************************************************************************************
			Private
		 *********************************************************************************************/

	private:

		BuildInfo();
		int 				_number;
		Version 			_version;
		Type	 			_type;
		Origin 				_origin;
		std::tm				_time;
	};
}


#endif //AVARA3D_BUILDINFO_H
