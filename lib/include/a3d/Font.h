//
//  Font.h
//  avara3d
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_FONT_H
#define AVARA3D_FONT_H

#include <filesystem>
#include <memory>
#include <optional>

namespace a3d {

class Buffer;

/**
 * @brief Owns font file data and basic format metadata.
 *
 * Copying a Font deep-copies its Buffer; moving transfers the owned data.
 */
class Font {

public:
    // [Public Types]

    /** @brief Recognized font container types. */
    enum class Type : uint8_t {
        Unknown, ///< Unknown or unspecified font type.
        OTF,     ///< OpenType font data.
        TTF,     ///< TrueType font data.
    };

    // [Public Lifecycle Functions]

    /**
     * @brief Loads font data from @p path and derives its name and type from the path.
     *
     * The name is the filename stem. The .otf and .ttf extensions select OTF
     * and TTF respectively; other extensions leave the type Unknown.
     *
     * @throws std::runtime_error if the file cannot be opened, sized, or read.
     */
    explicit Font(const std::filesystem::path& path);

    /**
     * @brief Takes ownership of @p buffer as font data with no name or known type.
     */
    explicit Font(std::unique_ptr<Buffer> buffer);

    Font(const Font& other);
    Font& operator=(const Font& other);

    Font(Font&& other) noexcept;
    Font& operator=(Font&& other) noexcept;

    ~Font();

    // [Public Member Functions]

    /** @brief Returns the font name, or an empty optional when no name is known. */
    const std::optional<std::string>& name() const;

    /** @brief Returns the detected or assigned font type. */
    Type                              type() const;

    /** @brief Returns the owned font-data Buffer, or nullptr when no Buffer is owned. */
    const Buffer*                     buffer() const;

private:
    // [Private Member Variables]

    std::optional<std::string> _name;
    Type                       _type;
    std::unique_ptr<Buffer>    _buffer;
};

} // namespace a3d

#endif // AVARA3D_FONT_H
