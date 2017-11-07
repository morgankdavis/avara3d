//
//  Test.h
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
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
	class Scene;

	
	class Test {
		
	public:
		
		int run(const std::vector<std::string>& args);
		
		void windowWillUpdateCallback(Scene& scene, float deltaSeconds);
		void windowDidUpdateCallback(Scene& scene, float deltaSeconds);
		
	private:

		std::shared_ptr<InputManager>		m_inputManager;
		std::shared_ptr<Node>				m_cameraNode;
		std::shared_ptr<Node>				m_planeNode;
		std::shared_ptr<Node>				m_boxNode;
		std::shared_ptr<Node>				m_sphereNode;
	};
}


#endif /* Test_h */
