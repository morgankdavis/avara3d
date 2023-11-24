//
//  Example.h
//	avara-engine
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Example_h
#define Example_h


#include <memory>
#include <string>
#include <vector>

#include "ae.h"


namespace example {
	
	
	enum class TEST {
		TRAVERSAL,
		MATRIX,
		CONVENIENCE,
		EULER,
		REVERSE_EULER,
		ROTATION
	};


	class Example {
		
	public:
		
		int run(const std::vector<std::string>& args);
		
		void updateCallback(ae::RenderContext& renderContext, float time);
		void willRenderCallback(ae::RenderContext& renderContext, float time);
		void didRenderCallback(ae::RenderContext& renderContext, float time);
		
	private:

		TEST _test;

	};
}


#endif /* Example_h */
