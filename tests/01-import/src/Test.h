//
//  Test.h
//	avara-engine
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Test_h
#define Test_h


#include <string>
#include <vector>


namespace ae {


	class Node;
	class Scene;

	class Test {
		
	public:
		
		int run(const std::vector<std::string>& args);
		
		void windowWillUpdateCallback(Scene& scene, float deltaSeconds);
		void windowDidUpdateCallback(Scene& scene, float deltaSeconds);
		
	private:
		
		std::shared_ptr<Node>		parentNode;
		std::shared_ptr<Node>		teapotNode;
		std::shared_ptr<Node>		palmNode;
		std::shared_ptr<Node>		siameseNode;
		std::shared_ptr<Node>		torusNode;
		std::shared_ptr<Node>		suzanneNode;
	};
}


#endif /* Test_h */
