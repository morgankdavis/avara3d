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


namespace ae {


	class Node;
	class Scene;

	class Test {
		
	public:
		
		int run(const std::vector<std::string>& args);
		
		void windowUpdateCallback(Scene& scene, float time);
		void windowWillRenderCallback(Scene& scene, float time);
		void windowDidRenderCallback(Scene& scene, float time);
		
	private:
		
		std::shared_ptr<Node> parentNode;
		std::shared_ptr<Node> teapotNode;
		std::shared_ptr<Node> palmNode;
		std::shared_ptr<Node> siameseNode;
		std::shared_ptr<Node> torusNode;
		std::shared_ptr<Node> coneNode;
		std::shared_ptr<Node> suzanneNode;
		std::shared_ptr<Node> ballNode;
	};
}


#endif /* Test_h */
