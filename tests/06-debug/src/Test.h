//
//  Test.h
//	avara-engine
//
//  Created by Morgan Davis on 1/4/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Test_h
#define Test_h


#include <memory>
#include <string>
#include <vector>

//#include "Types.h"


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

		Window*								m_window;
		std::shared_ptr<InputManager>		m_inputManager;
		std::shared_ptr<Node>				m_cameraNode;
	};
}


#endif /* Test_h */
