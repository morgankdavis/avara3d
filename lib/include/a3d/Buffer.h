//
//  Buffer.h
//  avara3d
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BUFFER_H
#define AVARA3D_BUFFER_H

#include <filesystem>
#include <memory>
#include <vector>

namespace a3d {

    /**
 * @brief Container for a contiguous byte buffer.
 *
 * Copying a Buffer deep-copies its bytes; moving transfers the owned storage.
 */
class Buffer {

public:
    // [Public Lifecycle Functions]

        /**
     * @brief Reads the complete contents of @p path into a Buffer.
     *
     * @throws std::runtime_error if the file cannot be opened, sized, or read.
     */
    explicit Buffer(const std::filesystem::path& path);

        /** @brief Creates a Buffer by copying the bytes in @p buf. */
    explicit Buffer(const std::vector<std::byte>& buf);

        /** @brief Creates a Buffer containing @p size bytes copied from @p buf. */
    Buffer(const std::byte* buf, std::size_t size);

        /** @brief Creates a Buffer with storage for @p size bytes. */
    explicit Buffer(std::size_t size);

    Buffer(const Buffer& other);
    Buffer& operator=(const Buffer& other);

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    ~Buffer();

    // [Public Member Functions]

        /** @brief Returns a mutable pointer to the owned bytes, or nullptr when the Buffer is empty. */
    std::byte*  data() const;

        /** @brief Returns the buffer size in bytes. */
    std::size_t size() const;

        /** @brief Returns a mutable pointer to the owned bytes, equivalent to data(). */
    std::byte*  operator*() const;

        /**
     * @brief Returns the byte at @p idx without bounds checking.
     *
     * @param idx valid zero-based byte index.
     */
    std::byte   operator[](std::size_t idx) const;

private:
    // [Private Member Variables]

    std::unique_ptr<std::byte[]> _data;
    std::size_t                  _size;
};

}

#endif // AVARA3D_BUFFER_H
