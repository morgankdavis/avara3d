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
	class RenderContext;

	class Test {
		
	public:
		
		int run(const std::vector<std::string>& args);
		
		void updateCallback(RenderContext& renderContext, float time);
		void willRenderCallback(RenderContext& renderContext, float time);
		void didRenderCallback(RenderContext& renderContext, float time);
		
	private:
		
		std::shared_ptr<Node> 		m_parentNode;
	};
}


#endif /* Test_h */
