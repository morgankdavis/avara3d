//
//  Example.h
//	avara-engine
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Example_h
#define Example_h


#include <memory>
#include <string>
#include <vector>

#include "ae/ae.h"


namespace test {

	class Test {
		
	public:
		
		int run(const std::vector<std::string>& args);

		void updateCallback(ae::Scene& scene, double time);
		void willRenderCallback(ae::VisualWorld& world, double time);
		void didRenderCallback(ae::VisualWorld& world, double time);
		
	private:
		
		std::shared_ptr<ae::Logger>			_logger;

		std::shared_ptr<ae::Node>			_ambientLightNode;
		std::shared_ptr<ae::Node>			_pointLightNode;
		
		// temporary
		std::shared_ptr<ae::Node>			_palletNode;
		std::shared_ptr<ae::Node>			_siameseNode;
		std::shared_ptr<ae::Node>			_palmsNode;
	};
}


#endif /* Example_h */
