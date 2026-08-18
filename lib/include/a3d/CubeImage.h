//
//  CubeImage.h
//  avara3d
//
//  Created by Morgan Davis on 4/30/18.
//  Copyright © 2018-2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_CUBEIMAGE_H
#define AVARA3D_CUBEIMAGE_H

#include <array>
#include <cstdint>
#include <memory>

namespace a3d {

    class Image;

    class CubeImage {

    public:
        /// Public Types ///

        enum class Face : uint8_t {
            X_Pos,
            X_Neg,
            Y_Pos,
            Y_Neg,
            Z_Pos,
            Z_Neg
        };

        /// Public Lifecycle Functions ///

        explicit CubeImage(std::array<std::unique_ptr<Image>, 6> faces);

        CubeImage(const CubeImage&)            = delete;
        CubeImage& operator=(const CubeImage&) = delete;

        CubeImage(CubeImage&&) noexcept;
        CubeImage& operator=(CubeImage&&) noexcept;

        ~CubeImage();

        /// Public Member Functions ///

        unsigned width() const;
        unsigned height() const;
        unsigned bytesPerPixel() const;

        Image*   face(Face face) const;

    private:
        /// Private Member Variables ///

        std::array<std::unique_ptr<Image>, 6> _faces;

        unsigned                              _width;
        unsigned                              _height;
        unsigned                              _bytesPerPixel;
    };

}

#endif /* AVARA3D_CUBEIMAGE_H */
