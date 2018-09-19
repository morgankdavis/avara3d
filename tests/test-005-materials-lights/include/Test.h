//
//  Test.h
//	avara-engine
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
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
		void willRenderCallback(RenderContext& renderContext, float time);
		void didRenderCallback(RenderContext& renderContext, float time);
		
	private:
		
		std::shared_ptr<Logger>				m_logger;

        std::shared_ptr<Window>				m_window;
		std::shared_ptr<InputManager>		m_inputManager;
		std::shared_ptr<Node>				m_cameraNode;
		std::shared_ptr<Node>				m_ambientLightNode;
		std::shared_ptr<Node>				m_pointLightNode;
		
		// temporary
		std::shared_ptr<Node>				m_palletNode;
		std::shared_ptr<Node>				m_siameseNode;
		std::shared_ptr<Node>				m_palmsNode;
	};
}


#endif /* Test_h */
