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

    class Buffer {

    public:
        // [Public Lifecycle Functions]

        explicit Buffer(const std::filesystem::path& path);
        explicit Buffer(const std::vector<std::byte>& buf);
        Buffer(const std::byte* buf, std::size_t size);
        explicit Buffer(std::size_t size);

        Buffer(const Buffer& other);
        Buffer& operator=(const Buffer& other);

        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;

        ~Buffer();

        // [Public Member Functions]

        std::byte*  data() const;
        std::size_t size() const;

        std::byte*  operator*() const;
        std::byte   operator[](std::size_t idx) const;

    private:
        // [Private Member Variables]

        std::unique_ptr<std::byte[]> _data;
        std::size_t                  _size;
    };

}

#endif /* AVARA3D_BUFFER_H */
