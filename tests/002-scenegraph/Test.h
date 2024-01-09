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


namespace test {
	
	enum class TEST {
		TRAVERSAL,
		MATRIX,
		CONVENIENCE,
		EULER,
		REVERSE_EULER,
		ROTATION
	};


	class Test {
		
	public:
		
		int run(const std::vector<std::string>& args);

		void updateCallback(ae::Scene& scene, float time);
		void willRenderCallback(ae::VisualWorld& world, float time);
		void didRenderCallback(ae::VisualWorld& world, float time);
		
	private:

		TEST _test;
	};
}


#endif /* Example_h */
