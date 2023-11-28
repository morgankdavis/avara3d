//
//  Example.h
//	avara-engine
//
//  Created by Morgan Davis on 1/4/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Example_h
#define Example_h


#include <memory>
#include <string>
#include <vector>

#include "ae.h"


namespace example {


	class Example {
		
	public:
		
		int run(const std::vector<std::string>& args);

		void updateCallback(ae::Scene& scene, float time);
		void willRenderCallback(ae::VisualWorld& world, float time);
		void didRenderCallback(ae::VisualWorld& world, float time);
	};
}


#endif /* Example_h */
