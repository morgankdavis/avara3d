//
//  Image.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_IMAGE_H
#define AVARA3D_IMAGE_H

#include <filesystem>
#include <memory>

namespace a3d {

class Buffer;

/**
 * @brief Owns decoded pixel data and its dimensions.
 *
 * Images decoded from encoded file or Buffer data are converted to four-byte
 * RGBA pixels. They are vertically flipped by default to match A3D texture
 * coordinates. Copying an Image deep-copies its pixel buffer; moving transfers
 * the owned storage.
 */
class Image {

public:
    // [Public Lifecycle Functions]

    /**
     * @brief Loads and decodes an image from @p path.
     *
     * @param flipVertical vertically flips the decoded pixels when true.
     * @param flipHorizontal horizontally mirrors the decoded pixels when true.
     * @throws std::runtime_error if the file cannot be read or its image data cannot be decoded.
     */
    explicit Image(const std::filesystem::path& path, bool flipVertical = true, bool flipHorizontal = false);

    /**
     * @brief Decodes encoded image data from @p buffer.
     *
     * @param flipVertical vertically flips the decoded pixels when true.
     * @param flipHorizontal horizontally mirrors the decoded pixels when true.
     * @throws std::runtime_error if the image data cannot be decoded.
     */
    explicit Image(const Buffer& buffer, bool flipVertical = true, bool flipHorizontal = false);

    /**
     * @brief Takes ownership of an existing pixel buffer and its dimensions.
     *
     * The caller is responsible for supplying a buffer compatible with
     * @p width, @p height, and @p bytesPerPixel.
     *
     * @param flipVertical vertically flips the pixels when true.
     * @param flipHorizontal horizontally mirrors the pixels when true.
     * @throws std::runtime_error if horizontal flipping is requested for data that is not four bytes per pixel.
     */
    Image(std::unique_ptr<Buffer> buffer,
          unsigned                width,
          unsigned                height,
          unsigned                bytesPerPixel,
          bool                    flipVertical   = true,
          bool                    flipHorizontal = false);

    Image(const Image& other);
    Image& operator=(const Image& other);

    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;

    ~Image();

    // [Public Member Functions]

    /** @brief Returns the image width in pixels. */
    unsigned               width() const;

    /** @brief Returns the image height in pixels. */
    unsigned               height() const;

    /** @brief Returns the number of bytes stored for each pixel. */
    unsigned               bytesPerPixel() const;

    /**
     * @brief Returns a new Image with its color bytes inverted.
     *
     * For four-byte pixels, RGB is inverted while alpha is preserved. For
     * other pixel sizes, every stored byte is inverted.
     */
    std::unique_ptr<Image> inverted() const;

    /** @brief Returns the Buffer containing the owned pixel data. */
    const Buffer&          buffer() const;

    /**
     * @brief Writes the image pixels to a PNG file at @p path.
     *
     * @return true on success; false if the PNG could not be written.
     */
    bool                   writePNG(const std::filesystem::path& path) const;

private:
    // [Private Member Functions]

    void                    loadBuffer(const Buffer& buffer, bool flipVertical, bool flipHorizontal);
    void                    flipVertical(); // "flip"
    void                    flipHorizontal(); // "mirror"

    // [Private Member Variables]

    unsigned                _width;
    unsigned                _height;
    unsigned                _bytesPerPixel;
    std::unique_ptr<Buffer> _buffer;
};

}

#endif // AVARA3D_IMAGE_H
