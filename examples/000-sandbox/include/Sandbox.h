//
//  Sandbox.h
//	avara-engine
//
//  Created by Morgan Davis on 12/02/23.
//  Copyright © 2023 Morgan K Davis. All rights reserved.
//

#ifndef Sandbox_h
#define Sandbox_h


#include <memory>
#include <string>
#include <vector>

#include "ae.h"


namespace sandbox {


	class Sandbox {
	
	public:
		
		int run(const std::vector<std::string>& args);

		void updateCallback(ae::Scene& scene, float time);
		void willRenderCallback(ae::VisualWorld& world, float time);
		void didRenderCallback(ae::VisualWorld& world, float time);
		void didSimulatePhysicsCallback(ae::PhysicalWorld& world, float time);
		
	private:

		std::shared_ptr<ae::Logger>			_logger;
	};
}


#endif /* Sandbox_h */
