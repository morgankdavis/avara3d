//
//  Font.cc
//	avara-engine
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Font.h"

#include "Buffer.h"
#include "Logger.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Font::Font(boost::filesystem::path& path):
	m_name(boost::none),
	m_type(FONT_TYPE::UNKNOWN),
	m_buffer(nullptr) {
	
		m_name = path.stem().string();

		auto extension = path.extension().string();
		if (extension == "otf") {
			m_type = FONT_TYPE::OTF;
		}
		else if (extension == "otf") {
			m_type = FONT_TYPE::TTF;
		}
		
		m_buffer = make_shared<Buffer>(path);
}

Font::Font(shared_ptr<Buffer> buffer):
	m_name(boost::none),
	m_type(FONT_TYPE::UNKNOWN),
	m_buffer(buffer) {
	
}

Font::~Font() {
	AE_LOG_D("Destroying Font {:p}", (void*)this);
}

/*********************************************************************************************
     Public
 *********************************************************************************************/

boost::optional<string> Font::name() const {
	return m_name;
}

FONT_TYPE Font::type() const {
	return m_type;
}

shared_ptr<Buffer> Font::buffer() const {
	return m_buffer;
}
