//
//  Example.h
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Example_h
#define Example_h


#include <memory>
#include <string>
#include <vector>

#include "ae.h"


namespace test {

	class Test {
		
	public:
		
		int run(const std::vector<std::string>& args);

		void updateCallback(ae::Scene& scene, float time);
		void willRenderCallback(ae::VisualWorld& world, float time);
		void didRenderCallback(ae::VisualWorld& world, float time);
		
	private:
		
		std::shared_ptr<ae::Logger>				_logger;
	};
}


#endif /* Example_h */
