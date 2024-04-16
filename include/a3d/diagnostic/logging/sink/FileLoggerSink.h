//
//  FileLoggerSink.h
//  avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_FILELOGGERSINK_H
#define AVARA3D_FILELOGGERSINK_H


#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "a3d/diagnostic/logging/sink/LoggerSink.h"


namespace a3d {

	class FileLoggerSink : public LoggerSink {


		static constexpr int DEFAULT_MAX_FILES = 5;
		static constexpr int DEFAULT_MAX_FILESIZE = 1024 * 1024 * 1; // 1MB


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:
	public:

		// desktop log paths are relative to the executable
		// android log paths are relative to the app's internal storage directory
		explicit FileLoggerSink(const std::filesystem::path& relPath,
								int maxFiles = DEFAULT_MAX_FILES,
								int maxFilesize = DEFAULT_MAX_FILESIZE);
		~FileLoggerSink();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		const std::filesystem::path&		filepath() const;

		int 								maxFiles() const;
		int 								maxFilesize() const;

		void 								flush() override;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 								write(const char* message);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		void 								openStream();
		void 								checkRotate();
		void 								rotate();

		std::filesystem::path				_filepath;
		int									_maxFiles;
		int									_maxFilesize;
		std::shared_ptr<std::ofstream>		_fileStream;
	};
}

#endif //AVARA3D_FILELOGGERSINK_H
