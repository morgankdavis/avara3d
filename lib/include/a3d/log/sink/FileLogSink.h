//
//  FileLogSink.h
//  avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_LOG_SINK_FILELOGSINK_H
#define AVARA3D_LOG_SINK_FILELOGSINK_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "a3d/log/sink/LogSink.h"

namespace a3d::log {

    /**
     * @brief LogSink that appends output to a file and rotates older files by size.
     *
     * Rotation is checked after each write. Numbered backups are kept beside the
     * active file and older backups are removed as the configured file limit is reached.
     */
    class FileLogSink : public LogSink {

    public:
        // [Public Lifecycle Functions]

        /**
         * @brief Creates an append-mode file sink with size-based rotation.
         *
         * Missing parent directories are created when possible. @p maxFilesize is
         * measured in bytes; the defaults retain up to five files with a 1 MiB size
         * threshold for each active file.
         *
         * @throws std::invalid_argument if @p maxFiles or @p maxFilesize is not greater than zero.
         * @throws std::runtime_error if required directories cannot be created or the log file cannot be opened.
         */
        explicit FileLogSink(const std::filesystem::path& relPath,
                             int                          maxFiles    = DEFAULT_MAX_FILES,
                             int                          maxFilesize = DEFAULT_MAX_FILESIZE);

        FileLogSink(const FileLogSink&)            = delete;
        FileLogSink& operator=(const FileLogSink&) = delete;

        FileLogSink(FileLogSink&&)            = delete;
        FileLogSink& operator=(FileLogSink&&) = delete;

        ~FileLogSink() override;

        // [Public Member Functions]

        /** @brief Returns the path of the active log file. */
        const std::filesystem::path& filepath() const;

        /** @brief Returns the configured maximum number of retained log files. */
        int                          maxFiles() const;

        /** @brief Returns the configured rotation size threshold in bytes. */
        int                          maxFilesize() const;

        // [Public LogSink Member Functions]

        /** @brief Appends @p output to the active file and performs size-based rotation when needed. */
        void                         write(const std::string& output, Log::Level level) override;

        /** @brief Flushes buffered output to the active file. */
        void                         flush() override;

    private:
        // [Private Constants]

        static constexpr unsigned      DEFAULT_MAX_FILES    = 5;
        static constexpr unsigned      DEFAULT_MAX_FILESIZE = 1024 * 1024 * 1; // 1MB

        // [Private Member Functions]

        void                           openStream();
        void                           checkRotate();
        void                           rotate();

        // [Private Member Variables]

        std::filesystem::path          _filepath;
        int                            _maxFiles;
        int                            _maxFilesize;
        std::shared_ptr<std::ofstream> _fileStream;
    };

}

#endif // AVARA3D_LOG_SINK_FILELOGSINK_H
