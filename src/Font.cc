//
//  Font.cc
//  avara3d
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/Font.h"

#include <utility>

#include "a3d/Buffer.h"
#include "a3d/diagnostic/log/Log.h"

using namespace a3d;
using namespace std;

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

Font::Font(const filesystem::path& path):
	_name{},
	_type{FontType::Unknown},
	_buffer{} {
	
		_name = path.stem().string();

		auto extension = path.extension().string();
		if (extension == ".otf") {
			_type = FontType::OTF;
		}
		else if (extension == ".ttf") {
			_type = FontType::TTF;
		}
		
		_buffer = make_unique<Buffer>(path);
}

Font::Font(unique_ptr<Buffer> buffer):
	_name{},
	_type{FontType::Unknown},
	_buffer{std::move(buffer)} {
	
}

Font::~Font() {
	A3D_LOG_D("Destroying Font {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

const optional<string>& Font::name() const {
	return _name;
}

FontType Font::type() const {
	return _type;
}

const Buffer* Font::buffer() const {
	return _buffer.get();
}
