//
// Created by mkd on 11/9/23.
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


		static constexpr unsigned DEFAULT_MAX_FILES = 3;
		static constexpr unsigned DEFAULT_MAX_FILESIZE = 1024 * 1024 * 1; // 1MB


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:
	public:

		// desktop log paths are relative to the executable
		// android log paths are relative to the app's internal storage directory
		FileLoggerSink(std::filesystem::path relPath,
					   unsigned maxFiles = DEFAULT_MAX_FILES,
					   unsigned maxFilesize = DEFAULT_MAX_FILESIZE);
		~FileLoggerSink();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::filesystem::path 				filepath() const;

		unsigned 							maxFiles() const;
		unsigned 							maxFilesize() const;

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
		unsigned							_maxFiles;
		unsigned							_maxFilesize;
		std::shared_ptr<std::ofstream>		_fileStream;
	};
}

#endif //AVARA3D_FILELOGGERSINK_H
