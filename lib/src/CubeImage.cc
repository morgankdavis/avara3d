//
//  CubeImage.cc
//  avara3d
//
//  Created by Morgan Davis on 4/30/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/CubeImage.h"

#include <stdexcept>
#include <string>
#include <utility>

#include "a3d/Image.h"

using namespace a3d;
using namespace std;

/// Public Lifecycle Functions ///

CubeImage::CubeImage(array<unique_ptr<Image>, 6> faces):
    _faces {},
    _width {0},
    _height {0},
    _bytesPerPixel {0} {

    for (size_t i = 0; i < faces.size(); ++i) {
        if (!faces[i]) {
            throw std::invalid_argument("CubeImage: face[" + std::to_string(i) + "] is null");
        }
    }

    const unsigned w = faces[0]->width();
    const unsigned h = faces[0]->height();
    const unsigned bpp = faces[0]->bytesPerPixel();

    for (size_t i = 1; i < faces.size(); ++i) {
        if (faces[i]->width() != w || faces[i]->height() != h || faces[i]->bytesPerPixel() != bpp) {
            throw std::invalid_argument("CubeImage: all faces must have identical width/height/bytesPerPixel");
        }
    }

    _width = w;
    _height = h;
    _bytesPerPixel = bpp;

    _faces = std::move(faces);
}

CubeImage::CubeImage(CubeImage&&) noexcept = default;

CubeImage& CubeImage::operator=(CubeImage&&) noexcept = default;

CubeImage::~CubeImage() = default;

/// Public Member Functions ///

unsigned CubeImage::width() const {
    return _width;
}

unsigned CubeImage::height() const {
    return _height;
}

unsigned CubeImage::bytesPerPixel() const {
    return _bytesPerPixel;
}

Image* CubeImage::face(Face face) const {
    return _faces[(size_t) face].get();
}
