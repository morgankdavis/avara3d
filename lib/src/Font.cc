//
//  Font.cc
//  avara3d
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/Font.h"

#include <utility>

#include "a3d/Buffer.h"
#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;

/// Public Lifecycle Functions ///

Font::Font(const filesystem::path& path):
    _name {},
    _type {Type::Unknown},
    _buffer {} {

    _name = path.stem().string();

    auto extension = path.extension().string();
    if (extension == ".otf") {
        _type = Type::OTF;
    }
    else if (extension == ".ttf") {
        _type = Type::TTF;
    }

    _buffer = make_unique<Buffer>(path);
}

Font::Font(unique_ptr<Buffer> buffer):
    _name {},
    _type {Type::Unknown},
    _buffer {std::move(buffer)} {}

Font::Font(const Font& other):
    _name {other._name},
    _type {other._type},
    _buffer {other._buffer ? make_unique<Buffer>(*other._buffer) : nullptr} {}

Font& Font::operator=(const Font& other) {

    if (this == &other) {
        return *this;
    }

    auto buffer = other._buffer ? make_unique<Buffer>(*other._buffer) : nullptr;

    _name = other._name;
    _type = other._type;
    _buffer = std::move(buffer);

    return *this;
}

Font::Font(Font&& other) noexcept = default;

Font& Font::operator=(Font&& other) noexcept = default;

Font::~Font() {
    log::d()("Destroying Font {:p}", static_cast<void*>(this));
}

/// Public Member Functions ///

const optional<string>& Font::name() const {
    return _name;
}

Font::Type Font::type() const {
    return _type;
}

const Buffer* Font::buffer() const {
    return _buffer.get();
}
