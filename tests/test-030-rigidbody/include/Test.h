//
//  Test.h
//	avara-engine
//
//  Created by Morgan Davis on 1/27/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Test_h
#define Test_h


#include <memory>
#include <string>
#include <vector>


namespace ae {


	class InputManager;
	class Logger;
	class Node;
	class RenderContext;
	class Scene;
	class Window;

	
	class Test {
	
	public:
		
		int run(const std::vector<std::string>& args);
		
		void updateCallback(RenderContext& renderContext, float time);
		void didSimulatePhysicsCallback(RenderContext& renderContext, float time);
		void willRenderCallback(RenderContext& renderContext, float time);
		void didRenderCallback(RenderContext& renderContext, float time);
		
	private:

		std::shared_ptr<Logger>				m_logger;
		
		std::shared_ptr<Window>				m_window;
		std::shared_ptr<InputManager>		m_inputManager;
		std::shared_ptr<Node>				m_cameraNode;
		
		std::shared_ptr<Node>				m_dickNode;
		std::shared_ptr<Node>				m_duckSpinnerNode;
		std::shared_ptr<Node>				m_duckNode;
		std::shared_ptr<Node>				m_paddleNode;
		
		std::shared_ptr<Node>				m_fruit1Node;
	};
}


#endif /* Test_h */
