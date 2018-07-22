//
//  Test.h
//	avara-engine
//
//  Created by Morgan Davis on 1/27/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Test_h
#define Test_h


#include <memory>
#include <string>
#include <vector>




// cheating
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <glm/gtc/type_ptr.hpp>
#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btScalar.h> // btGetVersion() !


namespace ae {


	class InputManager;
	class Node;
	class RenderContext;
	class Scene;
	class Window;

	
	class Test {
	
	public:
		
		int run(const std::vector<std::string>& args);
		
		void updateCallback(RenderContext& renderContext, float time);
		void didSimulatePhysicsCallback(RenderContext& renderContext, float time);
		void willRenderCallback(RenderContext& renderContext, float time);
		void didRenderCallback(RenderContext& renderContext, float time);
		
	private:

		std::shared_ptr<Window>				m_window;
		std::shared_ptr<InputManager>		m_inputManager;
		std::shared_ptr<Node>				m_cameraNode;
		
		
		
		btBroadphaseInterface* m_broadphase;
		btCollisionConfiguration* m_collisionConfiguration;
		btCollisionDispatcher* m_dispatcher;
		btConstraintSolver* m_solver;
		btDynamicsWorld* m_world;
		
		
		std::vector<btDefaultMotionState*>	m_motionStates;
		std::vector<std::shared_ptr<Node>>	m_boxNodes;

	};
}


#endif /* Test_h */
