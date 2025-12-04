//
//  FileLogSink.h
//  avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_FILELOGSINK_H
#define AVARA3D_FILELOGSINK_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "a3d/diagnostic/log/sink/LogSink.h"

namespace a3d {

	class FileLogSink : public LogSink {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit FileLogSink(const std::filesystem::path& relPath,
							 int maxFiles = DEFAULT_MAX_FILES,
							 int maxFilesize = DEFAULT_MAX_FILESIZE);
		~FileLogSink() override;

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		const std::filesystem::path&		filepath() const;

		int 								maxFiles() const;
		int 								maxFilesize() const;

		void 								flush() override;

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

		void 								write(const std::string& output);

/*********************************************************************************************
	Private Constants
 *********************************************************************************************/

	private:

		static constexpr unsigned 			DEFAULT_MAX_FILES = 5;
		static constexpr unsigned 			DEFAULT_MAX_FILESIZE = 1024 * 1024 * 1; // 1MB

/*********************************************************************************************
	Private Member Functions
 *********************************************************************************************/

		void 								openStream();
		void 								checkRotate();
		void 								rotate();

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

		std::filesystem::path				_filepath;
		int									_maxFiles;
		int									_maxFilesize;
		std::shared_ptr<std::ofstream>		_fileStream;
	};
}

#endif //AVARA3D_FILELOGSINK_H
