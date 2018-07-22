//
//  Test.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/27/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Test.h"

#include <iostream>

#include <glm/glm.hpp>

#include "ae.h"
#include "Utilities.h"


#warning experimental
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>




using namespace ae;
using namespace ae::utils;
using namespace std;
using namespace std::placeholders;
using namespace glm;


constexpr bool					USE_HIGH_DPI =			true;
constexpr unsigned				WINDOW_WIDTH =			1280;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr ANTIALIASING_MODE		ANTIALIAS_MODE =		ANTIALIASING_MODE::NONE;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		true;
constexpr float					MOUSE_SENSITIVITY =		0.5;



/***************************************************************************************
     Static Prototypes
 ***************************************************************************************/



/***************************************************************************************
     Public
 ***************************************************************************************/

int Test::run(const vector<string>& args) {
	AE_INIT();
	
	Logger::Level(LOG_LEVEL::DEBUG_);
	
	AE_LOG->info("Test::run()");

	auto renderer = make_shared<OpenGLRenderer>();
	m_window = make_shared<Window>(static_pointer_cast<Renderer>(renderer),
								   FULLSCREEN,
								   WINDOW_WIDTH, WINDOW_HEIGHT,
								   USE_HIGH_DPI, ANTIALIAS_MODE);
	m_window->updateCallback(bind(&Test::updateCallback, this, _1, _2));
	m_window->didSimulatePhysicsCallback(bind(&Test::didSimulatePhysicsCallback, this, _1, _2));
	m_window->willRenderCallback(bind(&Test::willRenderCallback, this, _1, _2));
	m_window->didRenderCallback(bind(&Test::didRenderCallback, this, _1, _2));
	m_window->enableVSync(ENABLE_VSYNC);
	m_window->captureCursor(CAPTURE_CURSOR);

	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));
	
	
	
	// init bullet

	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_broadphase = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver();
	m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);

	
	
	
	// create falling boxes
	{
		constexpr unsigned OBJECT_ARRAY_SIZE_X = 2;
		constexpr unsigned OBJECT_ARRAY_SIZE_Y = 4;
		constexpr unsigned OBJECT_ARRAY_SIZE_Z = 2;
		constexpr unsigned SPACING = 1.0;
		constexpr unsigned DROP_HEIGHT = 40.0;
		
		for (int k=0; k<OBJECT_ARRAY_SIZE_Y; ++k) {
			for (int i=0;i <OBJECT_ARRAY_SIZE_X; ++i) {
				for(int j = 0; j<OBJECT_ARRAY_SIZE_Z; ++j) {
					
					vec3 position = {
						SPACING * i - (OBJECT_ARRAY_SIZE_X / 2.0),
						DROP_HEIGHT + SPACING * k - (OBJECT_ARRAY_SIZE_Y / 2.0),
						SPACING * j  - (OBJECT_ARRAY_SIZE_Z / 2.0) };
					
					// create bullet model
					{
						btBoxShape* boxShape = new btBoxShape(btVector3(1.0f/2.0, 1.0f/2.0, 1.0f/2.0));
						
						// (makes no difference)
						//boxShape->setMargin(0);
						
						btTransform transform;
						transform.setIdentity();
						transform.setOrigin(btVector3(position.x, position.y, position.z));
						btDefaultMotionState* motionState = new btDefaultMotionState(transform);
						
						m_motionStates.push_back(motionState);
						
						float mass = 2.0;
						btVector3 localInertia = {0, 0, 0};
						boxShape->calculateLocalInertia(mass, localInertia);
						btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, boxShape, localInertia);
						
						rbInfo.m_friction = 0.25;
						rbInfo.m_rollingFriction = 0.25;
						rbInfo.m_restitution = 0.25;
						
						btRigidBody* rigidBody = new btRigidBody(rbInfo);
						m_world->addRigidBody(rigidBody);
					}
					
					// create visual model
					{
						auto node = Node::GeometryNode(make_shared<Box>(1.0, 1.0, 1.0));
						node->position(position);
						
						m_boxNodes.push_back(node);
						
						scene->rootNode()->addChild(node);
					}
				}
			}
		}
		
	}

	
	
	// create ground box
	{
		constexpr float GROUND_DIM = 50.0;
		
		// create bullet model
		{
			btBoxShape* groundShape = new btBoxShape(btVector3(GROUND_DIM/2.0, 0.25/2.0, GROUND_DIM/2.0));

			// (makes no difference)
			//groundShape->setMargin(0);
			
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(0, 0, 0));

			btDefaultMotionState* motionState = new btDefaultMotionState(transform);
			
			btVector3 localInertia = {0, 0, 0};
			groundShape->calculateLocalInertia(0, localInertia);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, groundShape, localInertia);
			
			rbInfo.m_mass = 0;
			rbInfo.m_friction = 1.0;
			rbInfo.m_rollingFriction = 1.0;
			rbInfo.m_restitution = 0.1;
			
			btRigidBody* rigidBody = new btRigidBody(rbInfo);
			m_world->addRigidBody(rigidBody);
		}
		
		// create visual model
		{
			auto groundNode = make_shared<Node>("Box");
			groundNode->geometry(make_shared<Box>(GROUND_DIM, 0.25, GROUND_DIM));
			auto gridImage = TestImageNamed("grid10");
			auto planeDiffuseProperty = make_shared<MaterialProperty>(gridImage);
			planeDiffuseProperty->wrapS(WRAP_MODE::REPEAT);
			planeDiffuseProperty->wrapT(WRAP_MODE::REPEAT);
			planeDiffuseProperty->maxAnisotropy(16);
			planeDiffuseProperty->minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR);
			planeDiffuseProperty->magnificationFilter(FILTER_MODE::LINEAR);
			auto planeMaterial = make_shared<Material>(nullptr, planeDiffuseProperty, nullptr);
			planeMaterial->uvScale(GROUND_DIM/10.0);
			planeMaterial->doubleSided(true);
			groundNode->geometry()->addMaterial(planeMaterial);
			groundNode->position({0, 0, 0});
			scene->rootNode()->addChild(groundNode);
		}
	}
	
	

	
	

	auto background = make_shared<MaterialProperty>(TestCubeImageNamed("sky1", "png"));
	scene->background(background);

	//auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, make_shared<Color>(0.65, 0.65, 0.65, 1.0));
	auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, make_shared<Color>(0.75, 0.75, 0.75, 1.0));
	auto ambientLightNode = Node::LightNode(ambientLight);
	scene->rootNode()->addChild(ambientLightNode);

	auto pointLight = make_shared<Light>(LIGHT_TYPE::POINT, Color::DarkGray());
	//pointLight->attenuationFactor(0.000000015);
	pointLight->attenuationFactor(0.0);
	auto pointLightNode = Node::LightNode(pointLight);
	scene->rootNode()->addChild(pointLightNode);
	pointLightNode->position(vec3(35, 20, 7) * vec3(2.5, 2.5, 2.5));

	scene->fogStartDistance(50.0);
	scene->fogEndDistance(400.0);
	scene->fogDensityExponent(1.0);
	scene->fogColor(Color::LightGray());
	
	AE_LOG->info("*** SCENE EXTENT: {} ***", StringFromGLMVec3(scene->extent()));
	
	m_window->scene(scene);
	m_inputManager = m_window->inputManager();
	m_window->display();
	
	return 0;
}

/***************************************************************************************
     RenderContext Callbacks
 ***************************************************************************************/

void Test::updateCallback(RenderContext& renderContext, float time) {
	AE_LOG->trace("updateCallback(RenderContext&, float)");

	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;

	auto scene = renderContext.scene();
	

	
	
	// step the sumulation
	{
		m_world->stepSimulation(deltaSeconds, 10, 1.0/120.0);
	}
	
	
	// update visual model
	{
		for (unsigned i=0; i<m_motionStates.size(); ++i) {
			auto motionState = m_motionStates[i];
			auto boxNode = m_boxNodes[i];
			
			btTransform btWorldTransform;
			btWorldTransform.setIdentity();
			motionState->getWorldTransform(btWorldTransform);
			
			mat4 worldMat;
			btWorldTransform.getOpenGLMatrix(value_ptr(worldMat));
			
			boxNode->transform(worldMat);
		}
	}
	
	
	

	
	// get input
	
	auto mouseButtonsDown = m_inputManager->mouseButtonsDown();
	auto mouseButtonsPressed = m_inputManager->mouseButtonsPressed();
	auto keysDown = m_inputManager->keysDown();
	auto keysPressed = m_inputManager->keysPressed();
	
	if (keysPressed.count(KEY::ESCAPE)) {
		m_window->setShouldClose();
	}
	
	if (keysPressed.count(KEY::T)) {
		AE_LOG->info("TREE:\n{}", StringFromTree(*(renderContext.scene()->rootNode())));
	}

	if (keysPressed.count(KEY::F)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(),
															DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(),
														 DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
	}
	if (keysPressed.count(KEY::B)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(),
															DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(),
														 DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
	}
	if (keysPressed.count(KEY::I)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(),
															DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(),
														 DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
	}

	if (keysPressed.count(KEY::V)) {
		m_window->enableVSync(!(m_window->vSyncEnabled()));
	}
	
	if (keysPressed.count(KEY::BACKSLASH)) {
		SaveSnapshot(*m_window);
	}
	
	if (keysPressed.count(KEY::SLASH)) {
		m_window->captureCursor(!(m_window->cursorCaptured()));
	}
	
	if (keysPressed.count(KEY::R)) {
		if (!m_window->recordingGIF()) {
			StartGIFRecording(*m_window, 240, 8);
		}
		else {
			StopGIFRecording(*m_window);
		}
	}
	
	if (m_window->cursorCaptured()) {
		
		// mouselook
		
		vec2 mousePositionDelta = m_inputManager->mousePositionDelta();
		
		static const float mouseSensitivity = (1.0f / MOUSE_SENSITIVITY);
		
		if (!m_cameraNode) {
			for (auto n : scene->rootNode()->children(false)) {
				if (n->camera()) {
					m_cameraNode = n;
					break;
				}
			}
		}
		
		if (m_cameraNode) {
			
			// look
			
			vec3 camForward = m_cameraNode->worldForward();
			vec3 camRight = m_cameraNode->worldRight();
			vec3 camUp = m_cameraNode->worldUp();
			
			float deltaRotX = atan(deltaSeconds * mousePositionDelta.x / mouseSensitivity);
			float deltaRotY = atan(deltaSeconds * mousePositionDelta.y / mouseSensitivity);
			
			//		float deltaRotX = deltaSeconds * mousePositionDelta.x / mouseSensitivity;
			//		float deltaRotY = deltaSeconds * mousePositionDelta.y / mouseSensitivity;
			
			vec3 angles = m_cameraNode->eulerAngles();
			m_cameraNode->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));
			
			// move
			
			//		const static float MOVE_SPEED = 5.0f; // units/sec
			static float MOVE_SPEED = 0;
			if (!MOVE_SPEED) MOVE_SPEED = Max(scene->extent());
			
			auto keysDown = m_inputManager->keysDown();
			
			float moveMultiplier = 1.0;
			if (keysDown.count(KEY::LEFT_SHIFT)) {
				moveMultiplier = 2.0;
			}
			
			if (keysDown.count(KEY::W) || mouseButtonsDown.count(MOUSE_BUTTON::FOUR)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camForward;
				m_cameraNode->position(m_cameraNode->position() + positionDelta);
			}
			else if (keysDown.count(KEY::S)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * -camForward;
				m_cameraNode->position(m_cameraNode->position() + positionDelta);
			}
			
			if (keysDown.count(KEY::A)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * -camRight;
				m_cameraNode->position(m_cameraNode->position() + positionDelta);
			}
			else if (keysDown.count(KEY::D)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camRight;
				m_cameraNode->position(m_cameraNode->position() + positionDelta);
			}
			
			if (keysDown.count(KEY::SPACE)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camUp;
				m_cameraNode->position(m_cameraNode->position() + positionDelta);
			}
		}
	}
}

void Test::didSimulatePhysicsCallback(RenderContext& renderContext, float time) {
	AE_LOG->trace("didSimulatePhysicsCallback(RenderContext&, float)");
}

void Test::willRenderCallback(RenderContext& renderContext, float time) {
	AE_LOG->trace("willRenderCallback(RenderContext&, float)");
}

void Test::didRenderCallback(RenderContext& renderContext, float time) {
	AE_LOG->trace("didRenderCallback(RenderContext&, float)");
}

/***************************************************************************************
     Static
 ***************************************************************************************/
