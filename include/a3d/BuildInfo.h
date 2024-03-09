//
// Created by mkd on 1/19/24.
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

		enum class TYPE {
			DEBUG,
			RELEASE
		};

		enum class ORIGIN {
			CI,
			ADHOC
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
		TYPE 			type() const;
		ORIGIN 			origin() const;
		const std::tm&	time() const;

		/*********************************************************************************************
			Private
		 *********************************************************************************************/

	private:

		BuildInfo();
		int 				_number;
		Version 			_version;
		TYPE	 			_type;
		ORIGIN 				_origin;
		std::tm				_time;
	};
}


#endif //AVARA3D_BUILDINFO_H
