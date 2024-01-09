//
//  Example.h
//	avara-engine
//
//  Created by Morgan Davis on 1/27/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
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
		void didSimulatePhysicsCallback(ae::PhysicalWorld& world, float time);
		
	private:

		std::shared_ptr<ae::Logger>			_logger;

		std::shared_ptr<ae::Node>			_palmNode;
		std::shared_ptr<ae::Node>			_duckSpinnerNode;
		std::shared_ptr<ae::Node>			_duckNode;
		std::shared_ptr<ae::Node>			_paddleNode;
		
		std::shared_ptr<ae::Node>			_fruit1Node;

//		std::shared_ptr<ae::Node>			_testNode;
	};
}


#endif /* Example_h */
