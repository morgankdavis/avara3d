//
//  Test.h
//	avara-engine
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Test_h
#define Test_h


#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>


namespace ae {


	class InputManager;
	class Node;
	class Scene;
	class Window;

	
	class Test {
		
	public:
		
		int run(const std::vector<std::string>& args);
		
		void windowUpdateCallback(Scene& scene, float time);
		void windowWillRenderCallback(Scene& scene, float time);
		void windowDidRenderCallback(Scene& scene, float time);
		
	private:
		
		std::shared_ptr<Window>				m_window;
		std::shared_ptr<InputManager>		m_inputManager;
		std::shared_ptr<Node>				m_cameraNode;
		std::shared_ptr<Node>				m_suzanneNode;
	};
}


#endif /* Test_h */
