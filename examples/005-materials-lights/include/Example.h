//
//  Example.h
//	avara-engine
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Example_h
#define Example_h


#include <memory>
#include <string>
#include <vector>

#include "ae.h"


namespace example {


	class Example {
		
	public:
		
		int run(const std::vector<std::string>& args);

		void updateCallback(ae::RenderContext& renderContext, float time);
		void willRenderCallback(ae::RenderContext& renderContext, float time);
		void didRenderCallback(ae::RenderContext& renderContext, float time);
		
	private:
		
		std::shared_ptr<ae::Logger>			m_logger;

        std::shared_ptr<ae::Window>			m_window;
		std::shared_ptr<ae::InputManager>	m_inputManager;
		std::shared_ptr<ae::Node>			m_cameraNode;
		std::shared_ptr<ae::Node>			m_ambientLightNode;
		std::shared_ptr<ae::Node>			m_pointLightNode;
		
		// temporary
		std::shared_ptr<ae::Node>			m_palletNode;
		std::shared_ptr<ae::Node>			m_siameseNode;
		std::shared_ptr<ae::Node>			m_palmsNode;
	};
}


#endif /* Example_h */
