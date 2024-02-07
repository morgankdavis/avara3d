//
//  Font.cc
//	avara-engine
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "ae/Font.h"

#include "ae/Buffer.h"
#include "ae/diagnostic/logging/Logger.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Font::Font(filesystem::path& path):
	_name(nullopt),
	_type(FontType::Unknown),
	_buffer(nullptr) {
	
		_name = path.stem().string();

		auto extension = path.extension().string();
		if (extension == "otf") {
			_type = FontType::OTF;
		}
		else if (extension == "otf") {
			_type = FontType::TTF;
		}
		
		_buffer = make_shared<Buffer>(path);
}

Font::Font(shared_ptr<Buffer> buffer):
	_name(nullopt),
	_type(FontType::Unknown),
	_buffer(buffer) {
	
}

Font::~Font() {
	AE_LOG_D("Destroying Font {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

optional<string> Font::name() const {
	return _name;
}

FontType Font::type() const {
	return _type;
}

shared_ptr<Buffer> Font::buffer() const {
	return _buffer;
}
