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
	
	
	typedef enum {
		MatrixTestCase,
		Convenience1TestCase,
		EulerTestCase,
		ReverseEulerTestCase,
		RotationAnimationTestCase
	} TestCase;
	


	class Scene;

	class Test {
		
	public:
		
		int run(const std::vector<std::string>& args);
		
		void windowUpdateCallback(Scene& scene, float time);
		void windowWillRenderCallback(Scene& scene, float time);
		void windowDidRenderCallback(Scene& scene, float time);
		
	private:
		
		TestCase TEST;

	};
}


#endif /* Test_h */
