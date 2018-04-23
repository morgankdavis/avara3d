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
	class Node;
	class Renderer;
	class Scene;
	class Window;

	
	class Test {
		
	public:
		
		int run(const std::vector<std::string>& args);
		
		void rendererUpdateCallback(Renderer& renderer, Scene& scene, float time);
		void rendererWillRenderCallback(Renderer& renderer, Scene& scene, float time);
		void rendererDidRenderCallback(Renderer& renderer, Scene& scene, float time);
		
	private:

        std::shared_ptr<Window>				m_window;
		std::shared_ptr<InputManager>		m_inputManager;
		std::shared_ptr<Node>				m_cameraNode;
		std::shared_ptr<Node>				m_ambientLightNode;
		std::shared_ptr<Node>				m_pointLightNode;
	};
}


#endif /* Test_h */
