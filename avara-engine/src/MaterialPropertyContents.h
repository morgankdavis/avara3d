//
//  MaterialPropertyContents.h
//	avara-engine
//
//  Created by Morgan Davis on 4/30/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef MaterialPropertyContents_h
#define MaterialPropertyContents_h


#include <memory>


namespace ae {

	class MaterialPropertyContents: public std::enable_shared_from_this<MaterialPropertyContents> {
		
	public:
		
		MaterialPropertyContents();
		virtual ~MaterialPropertyContents();
	};
}

#endif /* MaterialPropertyContents_h */
