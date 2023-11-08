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

		void updateCallback(ae::RenderContext& renderContext, float time);
		void willRenderCallback(ae::RenderContext& renderContext, float time);
		void didRenderCallback(ae::RenderContext& renderContext, float time);
		
	private:

		std::shared_ptr<ae::Logger>				_logger;
		std::shared_ptr<ae::Window>				_window;
		std::shared_ptr<ae::InputManager>		_inputManager;
		std::shared_ptr<ae::Node>				_cameraNode;
	};
}


#endif /* Example_h */
