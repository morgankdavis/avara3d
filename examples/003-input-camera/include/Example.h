//
//  Example.h
//	avara-engine
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Example_h
#define Example_h


#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "ae.h"


namespace example {


	class Example {
		
	public:
		
		int run(const std::vector<std::string>& args);
		
		void updateCallback(ae::RenderContext& renderContext, float time);
		void willRenderCallback(ae::RenderContext& renderContext, float time);
		void didRenderCallback(ae::RenderContext& renderContext, float time);
		
	private:
		
		std::shared_ptr<ae::Window>				m_window;
		std::shared_ptr<ae::InputManager>		m_inputManager;
		std::shared_ptr<ae::Node>				m_cameraNode;
	};
}


#endif /* Example_h */
