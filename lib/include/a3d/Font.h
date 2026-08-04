//
//  Font.h
//  avara3d
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_FONT_H
#define AVARA3D_FONT_H

#include <filesystem>
#include <memory>
#include <optional>

namespace a3d {

    class Buffer;

    class Font {

    public:
        /// Public Types ///

        enum class Type : uint8_t {
            Unknown,
            OTF,
            TTF,
        };

        /// Public Lifecycle Functions ///

        explicit Font(const std::filesystem::path& path);
        explicit Font(std::unique_ptr<Buffer> buffer);

        Font(const Font& other);
        Font& operator=(const Font& other);

        Font(Font&& other) noexcept;
        Font& operator=(Font&& other) noexcept;

        ~Font();

        /// Public Member Functions ///

        const std::optional<std::string>& name() const;
        Type                              type() const;
        const Buffer*                     buffer() const;

    private:
        /// Private Member Variables ///

        std::optional<std::string> _name;
        Type                       _type;
        std::unique_ptr<Buffer>    _buffer;
    };

}

#endif /* AVARA3D_FONT_H */
