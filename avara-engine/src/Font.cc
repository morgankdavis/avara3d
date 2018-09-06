//
//  Font.cc
//	avara-engine
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Font.h"


using namespace ae;
using namespace std;


/**************************************************************************************	
	Lifecycle
 ***************************************************************************************/

Font::Font(boost::filesystem::path path) {
	
}

/***************************************************************************************
     Public
 ***************************************************************************************/

string Font::name() const {
	
	return m_name;
}
