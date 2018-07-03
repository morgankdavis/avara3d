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


using namespace ae;
using namespace ae::utils;
using namespace std;
using namespace std::placeholders;
using namespace glm;


constexpr bool					USE_HIGH_DPI =			true;
constexpr unsigned				WINDOW_WIDTH =			800;
constexpr unsigned				WINDOW_HEIGHT =			600;
constexpr bool					FULLSCREEN =			false;
constexpr ANTIALIASING_MODE		ANTIALIAS_MODE =		ANTIALIASING_MODE::NONE;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		true;
constexpr float					MOUSE_SENSITIVITY =		0.5;
constexpr float					PHYSICS_TIMESTEP =		1.0/180.0;


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
	m_window->captureCursor(true);
	m_window->debugOptions(DEBUG_OPTIONS::SHOW_STATS_OVERLAY);
	
	auto renderContext = static_pointer_cast<RenderContext>(m_window);
	renderContext->debugOptions(DEBUG_OPTIONS_ADD(renderContext->debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
	renderContext->debugOptions(DEBUG_OPTIONS_ADD(renderContext->debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
	
	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));
	

	auto physicsWorld = make_shared<PhysicsWorld>();
	physicsWorld->timestep(PHYSICS_TIMESTEP);
	scene->physicsWorld(physicsWorld);

	
	const float PLANE_LENGTH = 30.0;
	const float PLANE_WIDTH = 30.0;
	const float PLANE_HEIGHT = 0.5;
	auto planeNode = make_shared<Node>("Box");
	planeNode->geometry(make_shared<Box>(PLANE_LENGTH, PLANE_WIDTH, PLANE_HEIGHT));
	auto gridImage = TestImageNamed("grid10");
	//auto gridImage = TestImageNamed("grid10_512");
	auto planeMaterialProperty = make_shared<MaterialProperty>(gridImage);
	planeMaterialProperty->wrapS(WRAP_MODE::REPEAT);
	planeMaterialProperty->wrapT(WRAP_MODE::REPEAT);
	planeMaterialProperty->maxAnisotropy(16);
	planeMaterialProperty->minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR);
	planeMaterialProperty->magnificationFilter(FILTER_MODE::LINEAR);
	auto planeMaterial = make_shared<Material>(nullptr, planeMaterialProperty, nullptr);
	planeMaterial->uvScale(PLANE_LENGTH/10.0);
//	planeMaterial->doubleSided(true);
	planeNode->geometry()->addMaterial(planeMaterial);
	planeNode->rotation({1, 0, 0, radians(90.0)});
	planeNode->position({planeNode->position().x,
		planeNode->position().y - PLANE_LENGTH,
		planeNode->position().z});

	auto planePhysicsBody = PhysicsBody::StaticBody();
	planePhysicsBody->mass(0);
	planePhysicsBody->restitution(0.0);
	planePhysicsBody->friction(0.75);
	planePhysicsBody->rollingFriction(0.75);
	planeNode->physicsBody(planePhysicsBody);
	
	
	scene->rootNode()->addChild(planeNode);

	
	
	
	
	
	
	m_pinappleNode = TestSceneNamed("pineapple_lod/pinapple_lod", "obj")->rootNode();
	scene->rootNode()->addChild(m_pinappleNode);
	
	m_boxNode = Node::GeometryNode(make_shared<Box>(0.5, 0.5, 2));
	m_boxNode->position({10, 0, 0});
	
	m_sphereNode = Node::GeometryNode(make_shared<Sphere>(0.5, 3));
	m_sphereNode->physicsBody(PhysicsBody::DynamicBody());
	
	//scene->rootNode()->addChild(m_boxNode);
	//m_boxNode->addChild(m_sphereNode);
	m_boxNode->addChild(m_sphereNode);
	m_pinappleNode->addChild(m_boxNode);
	
	
	
	

//	auto background = make_shared<MaterialProperty>(TestCubeImageNamed("sky1", "png"));
//	scene->background(background);

	auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, make_shared<Color>(0.65, 0.65, 0.65, 1.0));
	auto ambientLightNode = Node::LightNode(ambientLight);
	scene->rootNode()->addChild(ambientLightNode);

	auto pointLight = make_shared<Light>(LIGHT_TYPE::POINT, Color::DarkGray());
	pointLight->attenuationFactor(0.0);
	auto pointLightNode = Node::LightNode(pointLight);
	scene->rootNode()->addChild(pointLightNode);

	pointLightNode->position({25, 25, 25});

	
	scene->fogStartDistance(100.0);
	scene->fogEndDistance(600.0);
	scene->fogDensityExponent(1.0);
	scene->fogColor(Color::LightGray());

	AE_LOG->info("*** SCENE EXTENT: {} ***", StringFromGLMVec3(scene->extent()));
	
	m_window->scene(scene);
	m_inputManager = m_window->inputManager();
	m_window->display();
	
	return 0;
}

/***************************************************************************************
     Window Callbacks
 ***************************************************************************************/

void Test::updateCallback(RenderContext& renderContext, float time) {
	AE_LOG->trace("updateCallback(RenderContext&, float)");
	
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	float rotationDeg = deltaSeconds * 15.0; // 30deg/sec
	
	m_pinappleNode->transform(rotate(m_pinappleNode->transform(),
								 radians(rotationDeg),
								 vec3(0.0f, 1.0f, 0.0f)));

	
	
	
	
	
	if (m_sphereNode) {
		AE_LOG->info("sphere position: {}", StringFromGLMVec3(m_sphereNode->position()));
		AE_LOG->info("sphere presentation position: {}", StringFromGLMVec3(m_sphereNode->presentation()->position()));
	}

	
	
	auto scene = renderContext.scene();
	
	// get input
	
	auto mouseButtonsDown = m_inputManager->mouseButtonsDown();
	auto mouseButtonsPressed = m_inputManager->mouseButtonsPressed();
	auto keysPressed = m_inputManager->keysPressed();
	
	
	if (keysPressed.count(KEY::RIGHT_BRACKET)) {
		if (m_boxNode) {
			m_boxNode->position({
				 -m_boxNode->position().x,
				m_boxNode->position().y,
				m_boxNode->position().z});
		}
	}
	
//	if (keysPressed.count(KEY::LEFT_BRACKET)) {
//		if (m_thingBNode) {
//			m_thingBNode->position({
//				m_thingBNode->position().x + 10,
//				m_thingBNode->position().y,
//				m_thingBNode->position().z});
//		}
//	}
	
	
	if (keysPressed.count(KEY::ESCAPE)) {
		m_window->setShouldClose();
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
	if (keysPressed.count(KEY::P)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(),
															DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(),
														 DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES));
		}
	}
	if (keysPressed.count(KEY::G)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(),
															DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(),
														 DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES));
		}
	}
	if (keysPressed.count(KEY::C)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(),
															DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(),
														 DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS));
		}
	}
	if (keysPressed.count(KEY::N)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(),
															DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(),
														 DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS));
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
	
	if (keysPressed.count(KEY::U)) {
		for (auto& n : scene->rootNode()->children(true)) {
			auto geometry = n->geometry();
			if (geometry) {
				auto physicsBody = n->physicsBody();
				if (physicsBody && physicsBody->type() != PHYSICS_BODY_TYPE::STATIC) {
					bool coin = Random(0, 1) == 1;
					if (coin) {
						n->removeFromParent();
					}
				}
			}
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
