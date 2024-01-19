//
// Created by mkd on 1/19/24.
//

#ifndef AVARA_ENGINE_BUILDINFO_H
#define AVARA_ENGINE_BUILDINFO_H


namespace ae {

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

		BuildInfo(const BuildInfo &other) = delete; // copy constructor
		BuildInfo &operator=(const BuildInfo &other) = delete; // copy assignment

		/*********************************************************************************************
			Public
		 *********************************************************************************************/

		int 			number() const;
		Version 		version() const;
		TYPE 			type() const;
		ORIGIN 			origin() const;
		//std::time&	time() const;

		/*********************************************************************************************
			Private
		 *********************************************************************************************/

	private:

		BuildInfo();
		int 				_number;
		Version 			_version;
		TYPE	 			_type;
		ORIGIN 				_origin;
		// https://stackoverflow.com/questions/3505352/portable-way-to-create-a-timestamp-in-c-c
		// std::time 		date; // ?
		static BuildInfo* 	_instance;
	};
}


#endif //AVARA_ENGINE_BUILDINFO_H
