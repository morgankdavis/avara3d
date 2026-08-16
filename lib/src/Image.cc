//
//  Image.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/Image.h"

#include <cstdint>
#include <format>
#include <stdexcept>
#include <utility>

#include <stb/image.h>
#include <stb/image_write.h>
#include <webp/decode.h>

#include "a3d/Buffer.h"
#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;

/// Public Lifecycle Functions ///

Image::Image(const filesystem::path& path, bool flipVertical, bool flipHorizontal):
    _buffer {},
    _width {0},
    _height {0},
    _bytesPerPixel {0} {

    try {
        auto buffer = Buffer(path);
        loadBuffer(buffer, flipVertical, flipHorizontal);
    }
    catch (const runtime_error& e) {
        throw runtime_error(format("Failed to load image '{}': {}", path.string(), e.what()));
    }
}

Image::Image(const Buffer& buffer, bool flipVertical, bool flipHorizontal):
    _buffer {},
    _width {0},
    _height {0},
    _bytesPerPixel {0} {

    loadBuffer(buffer, flipVertical, flipHorizontal);
}

Image::Image(unique_ptr<Buffer> buffer,
             unsigned           width,
             unsigned           height,
             unsigned           bytesPerPixel,
             bool               flipVertical,
             bool               flipHorizontal):
    _width {width},
    _height {height},
    _bytesPerPixel {bytesPerPixel},
    _buffer {std::move(buffer)} {

    if (flipVertical) {
        Image::flipVertical();
    }
    if (flipHorizontal) {
        Image::flipHorizontal();
    }
}

Image::Image(const Image& other):
    _width {other._width},
    _height {other._height},
    _bytesPerPixel {other._bytesPerPixel},
    _buffer {other._buffer ? make_unique<Buffer>(*other._buffer) : nullptr} {}

Image& Image::operator=(const Image& other) {

    if (this == &other) {
        return *this;
    }

    auto buffer = other._buffer ? make_unique<Buffer>(*other._buffer) : nullptr;

    _width = other._width;
    _height = other._height;
    _bytesPerPixel = other._bytesPerPixel;
    _buffer = std::move(buffer);

    return *this;
}

Image::Image(Image&& other) noexcept:
    _width {std::exchange(other._width, 0)},
    _height {std::exchange(other._height, 0)},
    _bytesPerPixel {std::exchange(other._bytesPerPixel, 0)},
    _buffer {std::move(other._buffer)} {}

Image& Image::operator=(Image&& other) noexcept {

    if (this == &other) {
        return *this;
    }

    _width = std::exchange(other._width, 0);
    _height = std::exchange(other._height, 0);
    _bytesPerPixel = std::exchange(other._bytesPerPixel, 0);
    _buffer = std::move(other._buffer);

    return *this;
}

Image::~Image() {
    log::d()("Destroying Image {:p}", static_cast<void*>(this));
}

/// Public Member Functions ///

unsigned Image::width() const {
    return _width;
}

unsigned Image::height() const {
    return _height;
}

unsigned Image::bytesPerPixel() const {
    return _bytesPerPixel;
}

unique_ptr<Image> Image::inverted() const {
    const unsigned rowBytes = _width * _bytesPerPixel;
    const size_t   size = static_cast<size_t>(rowBytes) * _height;

    auto  out = make_unique<Buffer>(size); // assuming you have a size-ctor
    auto* dst = reinterpret_cast<unsigned char*>(out->data());
    auto* src = reinterpret_cast<const unsigned char*>(_buffer->data());

    const bool hasAlpha = (_bytesPerPixel == 4);

    for (size_t i = 0; i < size; ++i) {
        if (hasAlpha && ((i % 4) == 3)) { // keep alpha
            dst[i] = src[i];
        }
        else {
            dst[i] = static_cast<unsigned char>(255u - src[i]);
        }
    }

    return std::make_unique<Image>(std::move(out), _width, _height, _bytesPerPixel, false, false);
}

const Buffer& Image::buffer() const {
    return *_buffer;
}

bool Image::writePNG(const filesystem::path& path) const {

    return stbi_write_png(path.string().c_str(), (int) _width, (int) _height, (int) _bytesPerPixel,
                          _buffer->data(), (int) (_width * _bytesPerPixel))
           != 0;
}

/// Private Member Functions ///

void Image::loadBuffer(const Buffer& inBuf, bool flipVertical, bool flipHorizontal) {

    // force to RGBA / 4 bytes per pixel
    // (STB fills this with the ACTUAL BPP in the file, but pads to what we ask)
    constexpr unsigned BYTES_PER_PIXEL = 4;

    const auto* encodedData = reinterpret_cast<const uint8_t*>(inBuf.data());
    const auto  encodedSize = inBuf.size();

    int webpWidth = 0;
    int webpHeight = 0;

    if (WebPGetInfo(encodedData, encodedSize, &webpWidth, &webpHeight)) {

        const size_t rowBytes = static_cast<size_t>(webpWidth) * BYTES_PER_PIXEL;
        const size_t bufferSize = rowBytes * static_cast<size_t>(webpHeight);

        auto buffer = make_unique<Buffer>(bufferSize);

        auto* decodedData =
            WebPDecodeRGBAInto(encodedData, encodedSize, reinterpret_cast<uint8_t*>(buffer->data()), bufferSize,
                               static_cast<int>(rowBytes));

        if (!decodedData) {
            throw runtime_error("WebP decode failed.");
        }

        _buffer = std::move(buffer);
        _width = static_cast<unsigned>(webpWidth);
        _height = static_cast<unsigned>(webpHeight);
        _bytesPerPixel = BYTES_PER_PIXEL;

        log::d()("Loaded WebP image data. width: {}, height: {}, bytesPerPixel: {}", _width, _height,
                 _bytesPerPixel);
    }
    else {

        int width;
        int height;
        int bytesPerPixel;

        stbi_uc* imgData = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(inBuf.data()),
                                                 static_cast<int>(inBuf.size()), &width, &height,
                                                 &bytesPerPixel, STBI_rgb_alpha);

        if (!imgData) {
            const char* failureReason = stbi_failure_reason();

            if (failureReason) {
                throw runtime_error(format("Image decode failed: {}", failureReason));
            }

            throw runtime_error("Image decode failed.");
        }

        _buffer =
            make_unique<Buffer>(reinterpret_cast<const std::byte*>(imgData),
                                static_cast<size_t>(width) * static_cast<size_t>(height) * BYTES_PER_PIXEL);

        stbi_image_free(imgData);

        _width = static_cast<unsigned>(width);
        _height = static_cast<unsigned>(height);
        _bytesPerPixel = BYTES_PER_PIXEL;

        log::d()("Loaded image data. width: {}, height: {}, bytesPerPixel: {}", _width, _height,
                 _bytesPerPixel);
    }

    if (flipVertical) {
        Image::flipVertical();
    }

    if (flipHorizontal) {
        Image::flipHorizontal();
    }
}

void Image::flipVertical() { // "flip"

    unsigned       widthInBytes = _width * _bytesPerPixel;
    unsigned char* top = nullptr;
    unsigned char* bottom = nullptr;
    unsigned char  temp = 0;
    unsigned       halfHeight = _height / 2;

    auto dPtr = reinterpret_cast<unsigned char*>(_buffer->data());

    for (unsigned r = 0; r < halfHeight; ++r) {

        top = dPtr + r * widthInBytes;
        bottom = dPtr + (_height - r - 1) * widthInBytes;

        for (unsigned c = 0; c < widthInBytes; ++c) {

            temp = *top;
            *top = *bottom;
            *bottom = temp;
            ++top;
            ++bottom;
        }
    }
}

void Image::flipHorizontal() { // "mirror"

    if (_bytesPerPixel != 4) {
        throw std::runtime_error("flipHorizontal requires 4 bytesPerPixel");
    }

    auto*          d = reinterpret_cast<unsigned char*>(_buffer->data());
    const unsigned rowBytes = _width * 4;

    for (unsigned r = 0; r < _height; ++r) {
        unsigned char* row = d + r * rowBytes;
        for (unsigned x = 0; x < _width / 2; ++x) {
            unsigned char* L = row + x * 4;
            unsigned char* R = row + (_width - 1 - x) * 4;
            for (int k = 0; k < 4; ++k) {
                std::swap(L[k], R[k]);
            }
        }
    }
}
