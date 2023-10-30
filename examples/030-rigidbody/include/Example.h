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


namespace example {


	class Example {
	
	public:
		
		int run(const std::vector<std::string>& args);
		
		void updateCallback(ae::RenderContext& renderContext, float time);
		void didSimulatePhysicsCallback(ae::RenderContext& renderContext, float time);
		void willRenderCallback(ae::RenderContext& renderContext, float time);
		void didRenderCallback(ae::RenderContext& renderContext, float time);
		
	private:

		std::shared_ptr<ae::Logger>			m_logger;
		
		std::shared_ptr<ae::Window>			m_window;
		std::shared_ptr<ae::InputManager>	m_inputManager;
		std::shared_ptr<ae::Node>			m_cameraNode;

		std::shared_ptr<ae::Node>			m_palmNode;
		std::shared_ptr<ae::Node>			m_duckSpinnerNode;
		std::shared_ptr<ae::Node>			m_duckNode;
		std::shared_ptr<ae::Node>			m_paddleNode;
		
		std::shared_ptr<ae::Node>			m_fruit1Node;
	};
}


#endif /* Example_h */
