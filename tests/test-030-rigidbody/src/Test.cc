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
constexpr unsigned				WINDOW_WIDTH =			1280;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr ANTIALIASING_MODE		ANTIALIAS_MODE =		ANTIALIASING_MODE::NONE;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		true;
constexpr float					MOUSE_SENSITIVITY =		0.5;
constexpr float					PHYSICS_TIMESTEP =		1.0/120.0;


/***************************************************************************************
     Static Prototypes
 ***************************************************************************************/

static shared_ptr<Node> SpawnDuckFruit(Scene& scene, shared_ptr<Node> duckNode);
static shared_ptr<Node> AddSlurm(Scene& scene, vec3 location, vec4 rotation);
static shared_ptr<Node> ShootBall(Scene& scene, vec3 location, vec3 direction);
static shared_ptr<Node> AddBox(Scene& scene, vec3 location, shared_ptr<Color> color);
static shared_ptr<Node> AddCardboardBox(Scene& scene, vec3 location, vec4 rotation);
static void AddBoxes(Scene& scene);
static void AddCardboardBoxes(Scene& scene);
static void AddSlurms(Scene& scene);

/***************************************************************************************
     Public
 ***************************************************************************************/

int Test::run(const vector<string>& args) {
	AE_INIT();
	
	Logger::Level(LOG_LEVEL::DEBUG_);
	
	AE_LOG->info("Test::run()");

//	LOGGER_SINKS sinks = LOGGER_SINKS::NONE;
//	LOGGER_SINKS_ADD(sinks, LOGGER_SINKS::NATIVE);
//	m_logger = make_shared<Logger>("test30", sinks);
	
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
	m_window->debugOptions(DEBUG_OPTIONS::SHOW_STATS_OVERLAY);
	
//	auto renderContext = static_pointer_cast<RenderContext>(m_window);
//	renderContext->debugOptions(DEBUG_OPTIONS_ADD(renderContext->debugOptions(),
//												  DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES));
//	renderContext->debugOptions(DEBUG_OPTIONS_ADD(renderContext->debugOptions(),
//												  DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES));
	
//	auto renderContext = static_pointer_cast<RenderContext>(m_window);
//	renderContext->debugOptions(DEBUG_OPTIONS_ADD(renderContext->debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
//	renderContext->debugOptions(DEBUG_OPTIONS_ADD(renderContext->debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
	
	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));
	
	auto physicsWorld = make_shared<PhysicsWorld>();
	physicsWorld->timestep(PHYSICS_TIMESTEP);
	scene->physicsWorld(physicsWorld);

//	// GROUND BOX
//	
//	const float PLANE_LENGTH = 40.0;
//	const float PLANE_WIDTH = 40.0;
//	const float PLANE_HEIGHT = 0.25;
////	auto planeNode = make_shared<Node>(make_shared<Plane>(PLANE_WIDTH, PLANE_HEIGHT));
//	//auto planeNode = make_shared<Node>(make_shared<Box>(PLANE_LENGTH, PLANE_WIDTH, PLANE_HEIGHT));
//	auto planeNode = make_shared<Node>("Box");
//	planeNode->geometry(make_shared<Box>(PLANE_LENGTH, PLANE_WIDTH, PLANE_HEIGHT));
////	planeNode->geometry(make_shared<Plane>(10, 10));
//	auto gridImage = ImageNamed("grid10");
//	//auto gridImage = ImageNamed("grid10_512");
//	auto planeMaterialProperty = make_shared<MaterialProperty>(gridImage);
//	planeMaterialProperty->wrapS(WRAP_MODE::REPEAT);
//	planeMaterialProperty->wrapT(WRAP_MODE::REPEAT);
//	planeMaterialProperty->maxAnisotropy(16);
//	planeMaterialProperty->minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR);
//	planeMaterialProperty->magnificationFilter(FILTER_MODE::LINEAR);
//	auto planeMaterial = make_shared<Material>(nullptr, planeMaterialProperty, nullptr);
//	planeMaterial->uvScale(PLANE_LENGTH/10.0);
////	planeMaterial->doubleSided(true);
//	planeNode->geometry()->addMaterial(planeMaterial);
//	planeNode->rotation({1, 0, 0, radians(90.0)});
//	planeNode->position({planeNode->position().x, 0, planeNode->position().z});
//	
//	auto planePhysicsBody = PhysicsBody::StaticBody();
//	planePhysicsBody->mass(0);
//	planePhysicsBody->friction(100);
//	planePhysicsBody->rollingFriction(100);
//	planeNode->physicsBody(planePhysicsBody);
//	
//	scene->rootNode()->addChild(planeNode);
	
	
	
	
//	// GROUND BOX UNROTATED
//	
//	const float PLANE_EXTENT_X = 40.0;
//	const float PLANE_EXTENT_Y = 0.25;
//	const float PLANE_EXTENT_Z = 40.0;
//	auto boxNode = Node::GeometryNode(make_shared<Box>(PLANE_EXTENT_X, PLANE_EXTENT_Y, PLANE_EXTENT_Z));
////	auto boxGeometry = make_shared<Box>(PLANE_LENGTH, PLANE_WIDTH, PLANE_HEIGHT);
////	auto boxNode = make_shared<Node>();
//	//	planeNode->geometry(make_shared<Plane>(10, 10));
//	auto gridImage = ImageNamed("grid10");
//	//auto gridImage = ImageNamed("grid10_512");
//	auto boxMaterialProperty = make_shared<MaterialProperty>(gridImage);
//	boxMaterialProperty->wrapS(WRAP_MODE::REPEAT);
//	boxMaterialProperty->wrapT(WRAP_MODE::REPEAT);
//	boxMaterialProperty->maxAnisotropy(16);
//	boxMaterialProperty->minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR);
//	boxMaterialProperty->magnificationFilter(FILTER_MODE::LINEAR);
//	auto boxMaterial = make_shared<Material>(nullptr, boxMaterialProperty, nullptr);
//	boxMaterial->uvScale(PLANE_EXTENT_X/10.0);
//	//	planeMaterial->doubleSided(true);
//	boxNode->geometry()->addMaterial(boxMaterial);
//	
//	auto boxPhysicsBody = PhysicsBody::StaticBody();
//	boxPhysicsBody->mass(0);
//	boxPhysicsBody->friction(100);
//	boxPhysicsBody->rollingFriction(100);
//	boxNode->physicsBody(boxPhysicsBody);
//	
//	scene->rootNode()->addChild(boxNode);

	
	
	
	
	// GROUND PLANE
	
	const float PLANE_LENGTH = 40.0;
	const float PLANE_WIDTH = 40.0;
	const float PLANE_HEIGHT = 0.25;
	//	auto planeNode = make_shared<Node>(make_shared<Plane>(PLANE_WIDTH, PLANE_HEIGHT));
	//auto planeNode = make_shared<Node>(make_shared<Box>(PLANE_LENGTH, PLANE_WIDTH, PLANE_HEIGHT));
	auto planeNode = make_shared<Node>("Box");
//	planeNode->geometry(make_shared<Box>(PLANE_LENGTH, PLANE_WIDTH, PLANE_HEIGHT));
		planeNode->geometry(make_shared<Plane>(PLANE_LENGTH, PLANE_WIDTH));
	auto gridImage = ImageNamed("grid10");
	//auto gridImage = ImageNamed("grid10_512");
	auto planeMaterialProperty = make_shared<MaterialProperty>(gridImage);
	planeMaterialProperty->wrapS(WRAP_MODE::REPEAT);
	planeMaterialProperty->wrapT(WRAP_MODE::REPEAT);
	planeMaterialProperty->maxAnisotropy(16);
	planeMaterialProperty->minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR);
	planeMaterialProperty->magnificationFilter(FILTER_MODE::LINEAR);
	auto planeMaterial = make_shared<Material>(nullptr, planeMaterialProperty, nullptr);
	planeMaterial->uvScale(PLANE_LENGTH/10.0);
	planeMaterial->doubleSided(true);
	planeNode->geometry()->addMaterial(planeMaterial);
	planeNode->rotation({1, 0, 0, radians(3*90.0)});
	planeNode->position({planeNode->position().x, 0, planeNode->position().z});
	
	auto planePhysicsBody = PhysicsBody::StaticBody();
	planePhysicsBody->mass(0);
	planePhysicsBody->friction(1);
	planePhysicsBody->restitution(0.25);
	planeNode->physicsBody(planePhysicsBody);
	
	scene->rootNode()->addChild(planeNode);
	
	

	
	// add the duck
	
	m_duckNode = SceneNamed("rubberDuck/rubberDuck", "obj")->rootNode()->child("g duck", false);
	m_duckNode->position({4.5, 15, 0});
	//m_duckNode->scale({0.25, 0.25, 0.25});
//	m_duckNode->scale({0.33, 0.33, 0.33});
//#warning TEMPORARY workaround for physics scaling
//	for (auto& c : m_duckNode->children(true)) {
//		AE_LOG->debug("c tr: {}", StringFromGLMMat4(c->transform()));
//		c->geometry()->burnTransform(m_duckNode->transform(), true);
//		m_duckNode->transform(mat4(1.0));
//	}
	m_duckNode->physicsBody(PhysicsBody::KinematicBody());
	m_duckSpinnerNode = make_shared<Node>("duck spinner");
	m_duckSpinnerNode->addChild(m_duckNode);
	scene->rootNode()->addChild(m_duckSpinnerNode);
	
	
	
	// add the paddle
	
	m_paddleNode = Node::GeometryNode(make_shared<Box>(.5, 5, 20));
	auto paddleProperty = make_shared<MaterialProperty>(Color::Red());
	auto paddleMaterial = make_shared<Material>(nullptr, paddleProperty, nullptr);
	m_paddleNode->geometry()->addMaterial(paddleMaterial);
	m_paddleNode->position({15-.25, 2.5, 0});
	auto paddlePhysicsBody = PhysicsBody::KinematicBody();
//	paddlePhysicsBody->friction(100);
	paddlePhysicsBody->restitution(0.25);
	m_paddleNode->physicsBody(paddlePhysicsBody);
	scene->rootNode()->addChild(m_paddleNode);
	
	
	// add cardboard boxes
	AddCardboardBoxes(*scene);
	
	// add slurms
	AddSlurms(*scene);
	

	auto background = make_shared<MaterialProperty>(CubeImageNamed("sky1", "png"));
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
	
	
	// rotate the duck
	float rotationDeg = deltaSeconds * radians(30.0); // 30deg/sec
	
	if (m_duckSpinnerNode) {
		//auto duckRotation = m_duckSpinnerNode->rotation();
		//m_duckSpinnerNode->rotation({0, 1, 0, duckRotation.w + rotationDeg});
		auto duckSpinnerEuler = m_duckSpinnerNode->eulerAngles();
		m_duckSpinnerNode->eulerAngles({0, duckSpinnerEuler.y - rotationDeg, 0});
	}
	

	
	// get input
	
	auto mouseButtonsDown = m_inputManager->mouseButtonsDown();
	auto mouseButtonsPressed = m_inputManager->mouseButtonsPressed();
	auto keysDown = m_inputManager->keysDown();
	auto keysPressed = m_inputManager->keysPressed();
	auto cursorCaptured = true;
	if (dynamic_cast<Window*>(&renderContext)) {
		cursorCaptured = dynamic_cast<Window*>(&renderContext)->cursorCaptured();
	}
	
	if (keysPressed.count(KEY::ESCAPE)) {
		m_window->setShouldClose();
	}
	
	if (keysPressed.count(KEY::T)) {
		AE_LOG->info("TREE:\n{}", StringFromTree(*(renderContext.scene()->rootNode())));
	}
	
	
	// spawn duck fruit
	
	if (keysPressed.count(KEY::GRAVE_ACCENT)) {
		AddBoxes(*scene);
	}
	
	if (keysDown.count(KEY::TAB)) {
		auto fruitNode = SpawnDuckFruit(*scene, m_duckNode);
		if (!m_fruit1Node) {
			m_fruit1Node = fruitNode;
		}
	}
	
	if (keysPressed.count(KEY::J)) {
		if (m_fruit1Node) {
			m_fruit1Node->physicsBody()->affectedByGravity(!(m_fruit1Node->physicsBody()->affectedByGravity()));
		}
	}
	
	// move paddle
	
	static const float PADDLE_SPEED = 5.0; // m/s
	if (m_inputManager->keysDown().count(KEY::EQUAL)) {
		if (m_paddleNode) {
			auto p = m_paddleNode->position();
			m_paddleNode->position({p.x + deltaSeconds * PADDLE_SPEED, p.y, p.z});
		}
	}
	if (m_inputManager->keysDown().count(KEY::MINUS)) {
		if (m_paddleNode) {
			auto p = m_paddleNode->position();
			m_paddleNode->position({p.x - deltaSeconds * PADDLE_SPEED, p.y, p.z});
		}
	}
	
	
	if (cursorCaptured) {
		if (mouseButtonsPressed.count(MOUSE_BUTTON::ONE)) {
			ShootBall(*scene, m_window->pointOfView()->worldPosition(), m_window->pointOfView()->worldForward());
		}
		
		if (mouseButtonsDown.count(MOUSE_BUTTON::TWO)) {
			
			ShootBall(*scene, m_window->pointOfView()->worldPosition(), m_window->pointOfView()->worldForward());
		}
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
					bool coin = Uniform(0, 1) == 1;
					if (coin) {
						n->removeFromParent();
					}
				}
			}
		}
	}
	
	if (cursorCaptured) {
		
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

shared_ptr<Node> SpawnDuckFruit(Scene& scene, shared_ptr<Node> duckNode) {
	
	constexpr float SPAWN_RATE = 7.5; // pieces/sec
	
	float time = scene.renderContext().lock()->sceneTime();
	static float lastSSpawnTime = 0;
	if ((time - lastSSpawnTime) >= (1.0/SPAWN_RATE)) {
		
		
		unsigned fruitNum = Uniform(0, 5);
		shared_ptr<Node> node = nullptr;
		float mass = 1;
		
		switch (fruitNum) {
			case 0: {
				node = SceneNamed("cherry1_lod/cherry1_lod", "obj")->rootNode();
				mass = 0.05;
				break;
			}
			case 1: {
				node = SceneNamed("orange1_lod/orange1_lod", "obj")->rootNode();
				mass = 0.185;
				break;
			}
			case 2: {
				node = SceneNamed("pear_lod/pear_lod", "obj")->rootNode();
				mass = 0.24;
				break;
			}
			case 3: {
				node = SceneNamed("apple1_lod/apple1_lod", "obj")->rootNode();
				mass = 0.225;
				break;
			}
			case 4: {
				node = SceneNamed("banana_lod/banana_lod", "obj")->rootNode();
				mass = 0.14;
				break;
			}
			case 5: {
				node = SceneNamed("pineapple_lod/pinapple_lod", "obj")->rootNode();
				mass = 0.9;
				break;
			}
			default: return nullptr;
		}
		
//		node->scale({5.0, 5.0, 5.0});
		node->position(duckNode->worldPosition() + vec3(0.0, 1.0, 0.0));
		
		//	node->scale({3.0, 3.0, 3.0});
		//#warning TEMPORARY workaround for physics scaling
		//	for (auto& c : node->children(true)) {
		//		AE_LOG->debug("c tr: {}", StringFromGLMMat4(c->transform()));
		//		c->geometry()->burnTransform(node->transform(), true);
		//		node->transform(mat4(1.0));
		//	}
		
//		auto phyicsShape = make_shared<PhysicsShape>(node, PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON);
		auto phyicsShape = make_shared<PhysicsShape>(node, PHYSICS_SHAPE_TYPE::CONVEX_HULL);
		
		auto physicsBody = PhysicsBody::DynamicBody();
		physicsBody->shape(phyicsShape);
		
		
		
		physicsBody->mass(mass);
		physicsBody->restitution(0.25);
		physicsBody->friction(1);
		
		
		//physicsBody->angularDamping(0.1);
		//physicsBody->angularFactor({.1, .1, .1});
		
//		physicsBody->linearSleepingThreshold(0.1);
//		physicsBody->angularSleepingThreshold(.01);
		
		// add random factor
		
		float linearVelocityX = Uniform(-2.0f, 2.0f);
		float linearVelocityY = Uniform(5.0f, 12.0f);	
		float linearVelocityZ = Uniform(-2.0f, 2.0f);
		
		physicsBody->linearVelocity({linearVelocityX, linearVelocityY, linearVelocityZ});
		
		constexpr float ANGULAR_VARIANCE = 45.0; // deg/sec
		float angularVelocityX = Uniform(radians(-ANGULAR_VARIANCE), radians(ANGULAR_VARIANCE));	
		float angularVelocityY = Uniform(radians(-ANGULAR_VARIANCE), radians(ANGULAR_VARIANCE));	
		float angularVelocityZ = Uniform(radians(-ANGULAR_VARIANCE), radians(ANGULAR_VARIANCE));	
		
		physicsBody->angularVelocity({angularVelocityX, angularVelocityY, angularVelocityZ});
		
		node->physicsBody(physicsBody);
		
		scene.rootNode()->addChild(node);
		
		lastSSpawnTime = time;
		
		return node;
	}
	
	return nullptr;
}

shared_ptr<Node> AddSlurm(Scene& scene, vec3 location, vec4 rotation) {
	
	static auto fileScene = SceneNamed("slurm/slurm", "obj");
	static auto fileNode = fileScene->rootNode();
	static auto fileCanNode = fileNode->child("g slurm", false);
	static auto fileMaterials = fileCanNode->geometry()->materials();
	
	auto canNode = Node::GeometryNode(fileCanNode->geometry());
	for (auto& m : fileMaterials) {
		canNode->geometry()->addMaterial(m);
	}

	auto node = make_shared<Node>("Slurm");
	node->addChild(canNode);

	node->position(location);
	node->rotation(rotation);
	auto physicsBody = PhysicsBody::DynamicBody();
	physicsBody->mass(.4);
//	physicsBody->friction(5);
	node->physicsBody(physicsBody);
	scene.rootNode()->addChild(node);
	
	return node;
}

shared_ptr<Node> ShootBall(Scene& scene, vec3 location, vec3 direction) {
	
	constexpr float SHOOT_RATE = 20; // balls/sec
	
	float time = scene.renderContext().lock()->sceneTime();
	static float lastShootTime = 0;
	if ((time - lastShootTime) >= (1.0/SHOOT_RATE)) {

		static shared_ptr<Color> colors[] = {
			Color::White(),
			Color::Red(),
			Color::Orange(),
			Color::Yellow(),
			Color::Lime(),
			Color::Blue()
		};
		auto color = colors[Uniform(0, 5)];
		
		
		constexpr float BALL_RADIUS = 0.55;
		auto sphereGrometry = make_shared<Sphere>(BALL_RADIUS, 3);
		auto node = Node::GeometryNode(sphereGrometry);
		auto diffuseProperty = make_shared<MaterialProperty>(color);
		auto specularProperty = make_shared<MaterialProperty>(Color::White());
		auto material = make_shared<Material>(nullptr, diffuseProperty, specularProperty);
		material->specularExponent(125.0);
		node->geometry()->addMaterial(material);
		node->position(location);
		
		
//		auto node = SceneNamed("beachball1/beachball1", "obj")->rootNode()->children(false)[0];
//		node->position(location);
		
		
		auto physicsBody = PhysicsBody::DynamicBody();
		physicsBody->mass(0.2); // vollyball
		physicsBody->restitution(1.0);
		physicsBody->friction(0.015);
		physicsBody->rollingFriction(0.15);
//		physicsBody->friction(0);
//		physicsBody->rollingFriction(0);
		
		constexpr float BALL_VELOCITY = 45.0;
		constexpr float DIRECTION_VARIATION = 0.035f;
		auto variedDirection = direction + vec3(Uniform(-DIRECTION_VARIATION, DIRECTION_VARIATION),
												Uniform(-DIRECTION_VARIATION, DIRECTION_VARIATION),
												Uniform(-DIRECTION_VARIATION, DIRECTION_VARIATION));
		physicsBody->linearVelocity(normalize(variedDirection) * BALL_VELOCITY);
		
//		physicsBody->linearVelocity(normalize(direction) * BALL_VELOCITY);

//		constexpr float velocityVariation = 0.05f;
//		physicsBody->linearVelocity(direction + vec3(Uniform(-velocityVariation, velocityVariation),
//													 Uniform(-velocityVariation, velocityVariation),
//													 Uniform(-velocityVariation, velocityVariation)));
//		constexpr float angularVelocityVariation = radians(15.0f); // deg/sec
//		physicsBody->angularVelocity(vec3(Uniform(-angularVelocityVariation, angularVelocityVariation),
//										  Uniform(-angularVelocityVariation, angularVelocityVariation),
//										  Uniform(-angularVelocityVariation, angularVelocityVariation)));
		
		node->physicsBody(physicsBody);
		
		scene.rootNode()->addChild(node);
		
		lastShootTime = time;
		
		return node;
	}
	
	return nullptr;
}

shared_ptr<Node> AddBox(Scene& scene, vec3 location, shared_ptr<Color> color) {
	
	auto node = Node::GeometryNode(make_shared<Box>(1.0, 1.0, 1.0));
	auto materialProperty = make_shared<MaterialProperty>(color);
	auto material = make_shared<Material>(nullptr, materialProperty, nullptr);
	node->geometry()->addMaterial(material);
	node->position(location);
	
	auto physicsBody = PhysicsBody::DynamicBody();
	physicsBody->mass(1.0);
	physicsBody->restitution(0.1);
	physicsBody->friction(0.25);
	
	node->physicsBody(physicsBody);
	
	scene.rootNode()->addChild(node);
	
	return node;
}

shared_ptr<Node> AddCardboardBox(Scene& scene, vec3 location, vec4 rotation) {
	
	static auto fileScene = SceneNamed("cardboardBox2/cardboardBox2", "obj");
	static auto fileNode = fileScene->rootNode();
	static auto fileTapeNode = fileNode->child("g tape", false);
	static auto fileBoxNode = fileNode->child("g box", false);
	
	auto tapeNode = Node::GeometryNode(fileTapeNode->geometry());
	tapeNode->geometry()->addMaterial(fileTapeNode->geometry()->firstMaterial());
	auto boxNode = Node::GeometryNode(fileBoxNode->geometry());
	boxNode->geometry()->addMaterial(fileBoxNode->geometry()->firstMaterial());
	
	auto node = make_shared<Node>("Cardboard box");
	node->addChild(tapeNode);
	node->addChild(boxNode);

	node->position(location);
	node->rotation(rotation);
	
	auto physicsBody = PhysicsBody::DynamicBody();
	physicsBody->mass(0.1);
	physicsBody->restitution(0.1);
	physicsBody->friction(0.5);
	//physicsBody->angularFactor({2.0, 2.0, 2.0});
	node->physicsBody(physicsBody);
	
	scene.rootNode()->addChild(node);
	
	return node;
}

void AddBoxes(Scene& scene) {
	
	// 16 items
	constexpr unsigned OBJECT_ARRAY_SIZE_X = 2;
	constexpr unsigned OBJECT_ARRAY_SIZE_Y = 4;
	constexpr unsigned OBJECT_ARRAY_SIZE_Z = 2;
	constexpr float X_OFFSET = -10.0;
	constexpr float Z_OFFSET = 10.0;
	
	unsigned SPACING = 1.0;
	unsigned DROP_HEIGHT = 40.0;
	unsigned colorIndex = 0;
	auto colors = Color::Rainbow();
	for (int k=0; k<OBJECT_ARRAY_SIZE_Y; ++k) {
		for (int i=0;i <OBJECT_ARRAY_SIZE_X; ++i) {
			for(int j = 0; j<OBJECT_ARRAY_SIZE_Z; ++j) {
				auto color = colors[colorIndex + 4];
				++colorIndex;
				if (colorIndex + 4 > colors.size() -1 ) colorIndex = 0;
				vec3 position = { SPACING * i - (OBJECT_ARRAY_SIZE_X / 2.0) + X_OFFSET,
					DROP_HEIGHT + SPACING * k - (OBJECT_ARRAY_SIZE_Y / 2.0),
					SPACING * j  - (OBJECT_ARRAY_SIZE_Z / 2.0) + Z_OFFSET};
				AddBox(scene, position, color);
			}
		}
	}
}

void AddCardboardBoxes(Scene& scene) {
	
	constexpr unsigned HEIGHT = 7;
	
	const vec4 ROTATION = {0, 1, 0, radians(25.0)};
	
	constexpr float X_BASE = -15.0;
	constexpr float Y_BASE = 0.5;
	constexpr float Z_BASE = -10.0;
	
	constexpr float X_OFFSET = 0.75;
	constexpr float Y_OFFSET = 1.5;
	constexpr float Z_OFFSET = -0.36;
	
	for (unsigned r=0; r<HEIGHT; ++r) {
		float x = X_BASE + (X_OFFSET * r);
		float y = Y_BASE + (Y_OFFSET * r);
		float z = Z_BASE + (Z_OFFSET * r);
		
		for (unsigned i=0; i<HEIGHT-r; ++i) {
			AddCardboardBox(scene, {x, y, z}, ROTATION);
			x += 1.5; z -= 0.75;
		}
	}
}

void AddSlurms(Scene& scene) {

	float x = 15 - .25;
	float y = 5.5;
	float z = -9;
	
	for (unsigned i=0; i<19; ++i) {
		AddSlurm(scene, {x, y, z}, {0, 1, 0, radians((float)Uniform(0, 359))});
		z += 1;
	}
}
