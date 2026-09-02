//
//  CubeImage.h
//  avara3d
//
//  Created by Morgan Davis on 4/30/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_CUBEIMAGE_H
#define AVARA3D_CUBEIMAGE_H

#include <array>
#include <cstdint>
#include <memory>

namespace a3d {
class Image;

/**
 * @brief Owns the six images used as cubemap faces.
 *
 * All faces have identical width, height, and bytes-per-pixel values.
 */
class CubeImage {

public:
    // [Public Types]

    /** @brief Identifies a cubemap face by its outward axis direction. */
    enum class Face : uint8_t {
        X_Pos, ///< Positive X face.
        X_Neg, ///< Negative X face.
        Y_Pos, ///< Positive Y face.
        Y_Neg, ///< Negative Y face.
        Z_Pos, ///< Positive Z face.
        Z_Neg  ///< Negative Z face.
    };

    // [Public Lifecycle Functions]

    /**
     * @brief Takes ownership of the six cubemap face images.
     *
     * Array entries correspond to Face values in declaration order. Every face
     * must be non-null, and all faces must have identical dimensions and pixel
     * sizes.
     *
     * @throws std::invalid_argument if a face is null or the face dimensions or bytes-per-pixel values differ.
     */
    explicit CubeImage(std::array<std::unique_ptr<Image>, 6> faces);

    CubeImage(const CubeImage&)            = delete;
    CubeImage& operator=(const CubeImage&) = delete;

    CubeImage(CubeImage&&) noexcept;
    CubeImage& operator=(CubeImage&&) noexcept;

    ~CubeImage();

    // [Public Member Functions]

    /** @brief Returns the common face width in pixels. */
    unsigned width() const;

    /** @brief Returns the common face height in pixels. */
    unsigned height() const;

    /** @brief Returns the common number of bytes per pixel. */
    unsigned bytesPerPixel() const;

    /** @brief Returns the Image owned for @p face. */
    Image*   face(Face face) const;

private:
    // [Private Member Variables]

    std::array<std::unique_ptr<Image>, 6> _faces;

    unsigned                              _width;
    unsigned                              _height;
    unsigned                              _bytesPerPixel;
};
} // namespace a3d

#endif // AVARA3D_CUBEIMAGE_H
