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


#define ENABLE_HIGH_DPI        	true
#define WINDOW_WIDTH			800
#define WINDOW_HEIGHT			600
#define FULLSCREEN 				false
#define ANTIALIASING_MODE		AntialiasingMode_None
#define ENABLE_VSYNC			false
#define CAPTURE_CURSOR			true
#define MOUSE_SENSITIVITY		0.5
//#define PHYSICS_TIMESTEP		1.0/60.0
//#define PHYSICS_TIMESTEP		1.0/90.0
//#define PHYSICS_TIMESTEP		1.0/120.0
#define PHYSICS_TIMESTEP		1.0/180.0
//#define PHYSICS_TIMESTEP		1.0/240.0
//#define PHYSICS_TIMESTEP		1.0/480.0


/***************************************************************************************
     MARK:   Static
 **************************************************************************************/

void ShootBall(Scene& scene, vec3 location, vec3 direction) {
	
	cout << "location: " << location << endl;
	
	auto node = make_shared<Node>(make_shared<Sphere>(0.5 * .1, 3));
	auto materialProperty = make_shared<MaterialProperty>(make_shared<Color>(Color::Red()));
	auto material = make_shared<Material>(nullptr, materialProperty, nullptr);
	node->geometry()->addMaterial(material);
	node->position(location);
	
//	auto physicsShape = make_shared<PhysicsShape>(node->geometry(), PhysicsShapeType_ConvexHull);
//	auto physicsBody = make_shared<PhysicsBody>(PhysicsBodyType_Dynamic, physicsShape);
	auto physicsBody = PhysicsBody::DynamicBody();
	physicsBody->mass(100.0);
	physicsBody->restitution(0.45);
	physicsBody->friction(0.0);
	physicsBody->rollingFriction(0.0);
	physicsBody->velocity(direction * 50.0f);
	node->physicsBody(physicsBody);
	
	scene.rootNode()->addChildNode(node);
}

void AddObject(Scene& scene, vec3 location, shared_ptr<Color> color) {
	
	unsigned random = Random(0, 1);
	shared_ptr<Node> node = nullptr;
	if (random == 0) node = make_shared<Node>(make_shared<Box>(1.0, 1.0, 1.0));
	else node = make_shared<Node>(make_shared<Sphere>(0.5, 3));
	auto materialProperty = make_shared<MaterialProperty>(color);
	auto material = make_shared<Material>(nullptr, materialProperty, nullptr);
	node->geometry()->addMaterial(material);
	node->position(location);
	
//	auto physicsShape = make_shared<PhysicsShape>(node->geometry(), PhysicsShapeType_BoundingBox);
//	auto physicsBody = make_shared<PhysicsBody>(PhysicsBodyType_Dynamic, physicsShape);
	auto physicsBody = PhysicsBody::DynamicBody();
	physicsBody->mass(100.0);
	physicsBody->restitution(0.45);
	physicsBody->friction(0.5);
	physicsBody->rollingFriction(0.5);
	node->physicsBody(physicsBody);
	
	scene.rootNode()->addChildNode(node);
}

void AddBox(Scene& scene, vec3 location, shared_ptr<Color> color) {
	
	shared_ptr<Node> node = make_shared<Node>(make_shared<Box>(1.0, 1.0, 1.0));
	auto materialProperty = make_shared<MaterialProperty>(color);
	auto material = make_shared<Material>(nullptr, materialProperty, nullptr);
	node->geometry()->addMaterial(material);
	node->position(location);
	
//	auto physicsShape = make_shared<PhysicsShape>(node->geometry(), PhysicsShapeType_ConvexHull);
//	auto physicsBody = make_shared<PhysicsBody>(PhysicsBodyType_Dynamic, physicsShape);
	auto physicsBody = PhysicsBody::DynamicBody();
	physicsBody->mass(1.0);
	physicsBody->restitution(0.25);
	physicsBody->friction(0.25);
	physicsBody->rollingFriction(0.025);
	node->physicsBody(physicsBody);
	
	scene.rootNode()->addChildNode(node);
}

void AddCapsule(Scene& scene, vec3 location, shared_ptr<Color> color) {
	
	shared_ptr<Node> node = make_shared<Node>(make_shared<Capsule>(0.5, 0.5, 16, 16, 16));
	auto materialProperty = make_shared<MaterialProperty>(color);
	auto material = make_shared<Material>(nullptr, materialProperty, nullptr);
	node->geometry()->addMaterial(material);
	node->position(location);
	
	auto physicsBody = PhysicsBody::DynamicBody();
	physicsBody->mass(100.0);
	physicsBody->restitution(0.45);
	physicsBody->friction(0.5);
	physicsBody->rollingFriction(0.5);
	node->physicsBody(physicsBody);
	
	scene.rootNode()->addChildNode(node);
}

void AddCone(Scene& scene, vec3 location, shared_ptr<Color> color) {
	
	shared_ptr<Node> node = make_shared<Node>(make_shared<Cone>(0.5, 0.5, 16, 16));
	auto materialProperty = make_shared<MaterialProperty>(color);
	auto material = make_shared<Material>(nullptr, materialProperty, nullptr);
	node->geometry()->addMaterial(material);
	node->position(location);
	
	auto physicsBody = PhysicsBody::DynamicBody();
	physicsBody->mass(100.0);
	physicsBody->restitution(0.45);
	physicsBody->friction(0.5);
	physicsBody->rollingFriction(0.5);
	node->physicsBody(physicsBody);
	
	scene.rootNode()->addChildNode(node);
}

void AddCylinder(Scene& scene, vec3 location, shared_ptr<Color> color) {
	
	shared_ptr<Node> node = make_shared<Node>(make_shared<Cylinder>(0.5, 0.5, 16, 16));
	auto materialProperty = make_shared<MaterialProperty>(color);
	auto material = make_shared<Material>(nullptr, materialProperty, nullptr);
	node->geometry()->addMaterial(material);
	node->position(location);
	
	auto physicsBody = PhysicsBody::DynamicBody();
	physicsBody->mass(100.0);
	physicsBody->restitution(0.45);
	physicsBody->friction(0.5);
	physicsBody->rollingFriction(0.5);
	node->physicsBody(physicsBody);
	
	scene.rootNode()->addChildNode(node);
}

void AddFruit(Scene& scene, vec3 location) {
	
	//unsigned random = Random(0, 6);
	unsigned random = Random(2, 4);
	shared_ptr<Node> node = nullptr;
	
	switch (random) {
		case 0: node = TestSceneNamed("cherry1/cherry1", "obj")->rootNode()->childNodes(false)[0]; break;
		case 1: node = TestSceneNamed("orange1/orange1", "obj")->rootNode()->childNodes(false)[0]; break;
		case 2: node = TestSceneNamed("pear_lod/pear_lod", "obj")->rootNode()->childNodes(false)[0]; break;
		case 3: node = TestSceneNamed("apple1_lod/apple1_lod", "obj")->rootNode()->childNodes(false)[0]; break;
		case 4: node = TestSceneNamed("banana_lod/banana_lod", "obj")->rootNode()->childNodes(false)[0]; break;
		case 5: node = TestSceneNamed("pineapple_lod/pinapple_lod", "obj")->rootNode()->childNodes(false)[0]; break;
		default: return;
	}
	
	node->position(location);
	
	auto physicsBody = PhysicsBody::DynamicBody();
	//physicsBody->mass(100.0);
	physicsBody->mass(100.0);
	physicsBody->restitution(0.45);
	physicsBody->friction(0.75);
	physicsBody->rollingFriction(0.75);
	node->physicsBody(physicsBody);
	
	scene.rootNode()->addChildNode(node);
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

int Test::run(const vector<string>& args) {

	LoggerSink sinks = (LoggerSink)0;
	sinks = (LoggerSink)(sinks | (LoggerSink)LoggerSink_STDOUT);
	//sinks = (LoggerSink)(sinks | (LoggerSink)LoggerSink_MainFile);
	//sinks = (LoggerSink)(sinks | (LoggerSink)LoggerSink_NamedFile);
	m_logger = make_shared<Logger>("test30", sinks);
	
	auto window = Window(FULLSCREEN, WINDOW_WIDTH, WINDOW_HEIGHT, ENABLE_HIGH_DPI, ANTIALIASING_MODE);
	m_logger->info("Test::run()");
	
	window.updateCallback(bind(&Test::windowUpdateCallback, this, _1, _2));
	window.didSimulatePhysicsCallback(bind(&Test::didSimulatePhysicsCallback, this, _1, _2));
	window.willRenderCallback(bind(&Test::windowWillRenderCallback, this, _1, _2));
	window.didRenderCallback(bind(&Test::windowDidRenderCallback, this, _1, _2));
	window.captureCursor(CAPTURE_CURSOR);
	window.enableVSync(ENABLE_VSYNC);
	window.debugOptions(DebugOption_ShowStatsOveray);
	m_window = &window;

	
	auto scene = make_shared<Scene>();
	
	
	auto physicsWorld = make_shared<PhysicsWorld>();
	physicsWorld->timestep(PHYSICS_TIMESTEP);
	scene->physicsWorld(physicsWorld);
	
	
	const float PLANE_DIM = 10.0;
	//auto planeNode = make_shared<Node>(make_shared<Plane>(PLANE_DIM, PLANE_DIM));
	auto planeNode = make_shared<Node>(make_shared<Box>(PLANE_DIM, PLANE_DIM, 0.1));
	//auto gridImage = TestImageNamed("grid10");
	auto gridImage = TestImageNamed("grid10_512");
	auto planeMaterialProperty = make_shared<MaterialProperty>(gridImage);
	planeMaterialProperty->wrapS(WrapMode_Repeat);
	planeMaterialProperty->wrapT(WrapMode_Repeat);
	auto planeMaterial = make_shared<Material>(nullptr, planeMaterialProperty, nullptr);
	planeMaterial->uvScale(PLANE_DIM);
//	planeMaterial->uvScale(PLANE_DIM*0.1);
//	planeMaterial->doubleSided(true);
	planeNode->geometry()->addMaterial(planeMaterial);
	planeNode->rotation({1, 0, 0, radians(90.0)});
	planeNode->position({planeNode->position().x,
		planeNode->position().y - PLANE_DIM,
		planeNode->position().z});
	
	
//	auto placePhysicsShape = make_shared<PhysicsShape>(planeNode->geometry(), PhysicsShapeType_ConvexHull);
//	auto planePhysicsBody = make_shared<PhysicsBody>(PhysicsBodyType_Static, placePhysicsShape);
	auto planePhysicsBody = PhysicsBody::StaticBody();
	planePhysicsBody->mass(0);
	planePhysicsBody->restitution(0.25);
	planePhysicsBody->friction(0.75);
	planePhysicsBody->rollingFriction(0.75);
	planeNode->physicsBody(planePhysicsBody);
	
	
	scene->rootNode()->addChildNode(planeNode);
	
	
	
	
	// added random boxes and spheres
	
#define OBJECT_ARRAY_SIZE_X	2
#define OBJECT_ARRAY_SIZE_Y	4
#define OBJECT_ARRAY_SIZE_Z	2
	// -> 16
	
//#define OBJECT_ARRAY_SIZE_X	3
//#define OBJECT_ARRAY_SIZE_Y	4
//#define OBJECT_ARRAY_SIZE_Z	3
	// -> 36
	
//#define OBJECT_ARRAY_SIZE_X	4
//#define OBJECT_ARRAY_SIZE_Y	6
//#define OBJECT_ARRAY_SIZE_Z	4
	// -> 96
	
//#define OBJECT_ARRAY_SIZE_X	5
//#define OBJECT_ARRAY_SIZE_Y	7
//#define OBJECT_ARRAY_SIZE_Z	5
	// -> 175
	
//#define OBJECT_ARRAY_SIZE_X	6
//#define OBJECT_ARRAY_SIZE_Y	8
//#define OBJECT_ARRAY_SIZE_Z	6
	// -> 288

	
	unsigned SPACING = 1.0;
	unsigned DROP_HEIGHT = 5.0;
	unsigned colorIndex = 0;
	auto colors = Color::Rainbow();
	for (int k=0; k<OBJECT_ARRAY_SIZE_Y; ++k) {
		for (int i=0;i <OBJECT_ARRAY_SIZE_X; ++i) {
			for(int j = 0; j<OBJECT_ARRAY_SIZE_Z; ++j) {
				auto color = make_shared<Color>(colors[colorIndex + 4]);
				++colorIndex;
				if (colorIndex + 4 > colors.size() -1 ) colorIndex = 0;
				vec3 position = { SPACING * i - (OBJECT_ARRAY_SIZE_X / 2.0),
					DROP_HEIGHT + SPACING * k - (OBJECT_ARRAY_SIZE_Y / 2.0),
					SPACING * j  - (OBJECT_ARRAY_SIZE_Z / 2.0) };
				//AddObject(*scene, position, color);
				//AddBox(*scene, position, color);
				//AddCapsule(*scene, position, color);
				//AddCone(*scene, position, color);
				//AddCylinder(*scene, position, color);
				AddFruit(*scene, position);
			}
		}
	}
	
	
	
//	auto crateScene = TestSceneNamed("crate2/crate2", "obj");
//	vector<shared_ptr<Node>> crateNodes;
//	for (auto n : crateScene->rootNode()->childNodes(true)) {
//		if (n->geometry()) {
//			auto physicsShape = make_shared<PhysicsShape>(n->geometry(), PhysicsShapeType_ConvexHull);
//			auto physicsBody = make_shared<PhysicsBody>(PhysicsBodyType_Static, physicsShape);
//			physicsBody->mass(0);
//			physicsBody->restitution(0.5);
//			n->physicsBody(physicsBody);
//		}
//		n->position(n->position() + vec3(0, -20, 0));
//		crateNodes.emplace_back(n);
//	}
//	scene->rootNode()->addChildNodes(crateNodes);

	
//	auto pineappleScene = TestSceneNamed("pineapple/pinapple", "obj");
//	scene->rootNode()->addChildNode(pineappleScene->rootNode());
	
//	auto pineappleScene = TestSceneNamed("pineapple_lod/pinapple_lod", "obj");
//	auto pineappleNode = pineappleScene->rootNode()->childNodes(false)[0]; // NO GOOD
//	scene->rootNode()->addChildNode(pineappleNode);

	
//	auto banana1Scene = TestSceneNamed("banana_lod/banana_lod", "obj");
//	scene->rootNode()->addChildNode(banana1Scene->rootNode());

//	auto pearScene = TestSceneNamed("pear/pear", "obj");
//	scene->rootNode()->addChildNode(pearScene->rootNode());
	
//	auto apple1Scene = TestSceneNamed("apple1/apple1", "obj");
//	scene->rootNode()->addChildNode(apple1Scene->rootNode());

//	auto apple2Scene = TestSceneNamed("apple2/apple2", "obj");
//	scene->rootNode()->addChildNode(apple2Scene->rootNode());
	
//	auto orange1Scene = TestSceneNamed("orange1/orange1", "obj");
//	scene->rootNode()->addChildNode(orange1Scene->rootNode());
	
//	auto cherry1Scene = TestSceneNamed("cherry1/cherry1", "obj");
//	scene->rootNode()->addChildNode(cherry1Scene->rootNode());
	
//	auto cherry2Scene = TestSceneNamed("cherry2/cherry2", "obj");
//	scene->rootNode()->addChildNode(cherry2Scene->rootNode());
	
//	auto coke1Scene = TestSceneNamed("coke1/coke1", "obj");
//	scene->rootNode()->addChildNode(coke1Scene->rootNode());
	
//	auto slurmScene = TestSceneNamed("slurm/slurm", "obj");
//	scene->rootNode()->addChildNode(slurmScene->rootNode());

//	auto picnictableScene = TestSceneNamed("picnictable/picnictable", "obj");
//	scene->rootNode()->addChildNode(picnictableScene->rootNode());
	
	

	auto background = make_shared<MaterialProperty>(TestCubeNamed("sky1", "png"));
	scene->background(background);

	auto ambientLight = make_shared<Light>(LightType_Ambient, make_shared<Color>(0.5, 0.5, 0.5, 1.0));
	auto ambientLightNode = make_shared<Node>(ambientLight);
	scene->rootNode()->addChildNode(ambientLightNode);

	auto pointLight = make_shared<Light>(LightType_Point, make_shared<Color>(Color::LightGray()));
	//pointLight->attenuationFactor(0.000000015);
	pointLight->attenuationFactor(0.0);
	auto pointLightNode = make_shared<Node>(pointLight);
	scene->rootNode()->addChildNode(pointLightNode);

	pointLightNode->position({25, 25, 25});

//	auto materialProperty = make_shared<MaterialProperty>(pointLight->color());
//	auto material = make_shared<Material>();
//	material->name("LIGHT material");
//	material->emissive(materialProperty);
//	auto geometry = make_shared<Sphere>(3.5, 16);
//	geometry->addMaterial(material);
//	pointLightNode->geometry(geometry);


	scene->fogStartDistance(100.0);
	scene->fogEndDistance(600.0);
	scene->fogDensityExponent(1.0);
	scene->fogColor(make_shared<Color>(Color::LightGray()));

	AE_LOG->info("*** SCENE EXTENT: {} ***", StringFromGLMVec3(scene->extent()));
	
	window.scene(scene);
	m_inputManager = window.inputManager();
	window.display();
	
	return 0;
}

/***************************************************************************************
     MARK:   Window Callbacks
 **************************************************************************************/

void Test::windowUpdateCallback(Scene& scene, float time) {
	m_logger->trace("windowUpdateCallback()");
	
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	// get input
	
	auto mouseButtonsDown = m_inputManager->mouseButtonsDown();
	auto mouseButtonsPressed = m_inputManager->mouseButtonsPressed();
	auto keysPressed = m_inputManager->keysPressed();
	
	if (keysPressed.count(Key_Escape)) {
		exit(0);
	}
	
	if (mouseButtonsPressed.count(MouseButton_1)) {
		ShootBall(scene, m_window->pointOfView()->worldPosition(), m_window->pointOfView()->worldForward());
	}
	
	if (mouseButtonsDown.count(MouseButton_2)) {
		ShootBall(scene, m_window->pointOfView()->worldPosition(), m_window->pointOfView()->worldForward());
	}
	
	DebugOption options = (DebugOption)m_window->debugOptions();
	if (keysPressed.count(Key_F)) {
		if (m_window->debugOptions() & DebugOption_ShowWireframes) {
			m_window->debugOptions((DebugOption)(options & ~DebugOption_ShowWireframes));
		}
		else {
			m_window->debugOptions((DebugOption)(options | DebugOption_ShowWireframes));
		}
	}
	if (keysPressed.count(Key_B)) {
		if (m_window->debugOptions() & DebugOption_ShowBoundingBoxes) {
			m_window->debugOptions((DebugOption)(options & ~DebugOption_ShowBoundingBoxes));
		}
		else {
			m_window->debugOptions((DebugOption)(options | DebugOption_ShowBoundingBoxes));
		}
	}
	if (keysPressed.count(Key_I)) {
		if (m_window->debugOptions() & DebugOption_ShowStatsOveray) {
			m_window->debugOptions((DebugOption)(options & ~DebugOption_ShowStatsOveray));
		}
		else {
			m_window->debugOptions((DebugOption)(options | DebugOption_ShowStatsOveray));
		}
	}
	
	if (keysPressed.count(Key_P)) {
		if (m_window->debugOptions() & DebugOption_ShowPhysicsBoundingBoxes) {
			m_window->debugOptions((DebugOption)(options & ~DebugOption_ShowPhysicsBoundingBoxes));
		}
		else {
			m_window->debugOptions((DebugOption)(options | DebugOption_ShowPhysicsBoundingBoxes));
		}
	}
	
	if (keysPressed.count(Key_G)) {
		if (m_window->debugOptions() & DebugOption_ShowPhysicsWireframes) {
			m_window->debugOptions((DebugOption)(options & ~DebugOption_ShowPhysicsWireframes));
		}
		else {
			m_window->debugOptions((DebugOption)(options | DebugOption_ShowPhysicsWireframes));
		}
	}
	
	if (keysPressed.count(Key_C)) {
		if (m_window->debugOptions() & DebugOption_ShowPhysicsContactPoints) {
			m_window->debugOptions((DebugOption)(options & ~DebugOption_ShowPhysicsContactPoints));
		}
		else {
			m_window->debugOptions((DebugOption)(options | DebugOption_ShowPhysicsContactPoints));
		}
	}
	
	if (keysPressed.count(Key_N)) {
		if (m_window->debugOptions() & DebugOption_ShowPhysicsContactPoints) {
			m_window->debugOptions((DebugOption)(options & ~DebugOption_ShowPhysicsNormals));
		}
		else {
			m_window->debugOptions((DebugOption)(options | DebugOption_ShowPhysicsNormals));
		}
	}
	
	
	

//	DebugOption_ShowPhysicsNormals = 			1 << 9,
//	DebugOption_ShowPhysicsConstraints =		1 << 10,
//	DebugOption_ShowPhysicsConstraintLimits	=	1 >> 11

	if (keysPressed.count(Key_V)) {
		m_window->enableVSync(!(m_window->vSyncEnabled()));
	}
	
	if (keysPressed.count(Key_Backslash)) {
		SaveSnapshot(*m_window);
	}
	
	if (keysPressed.count(Key_R)) {
		if (!m_window->recordingGIF()) {
			StartGIFRecording(*m_window, 240, 8);
		}
		else {
			StopGIFRecording(*m_window);
		}
	}
	
	// mouselook
	
	vec2 mousePositionDelta = m_inputManager->mousePositionDelta();
	
	static const float mouseSensitivity = (1.0f / MOUSE_SENSITIVITY);
	
	if (!m_cameraNode) {
		for (auto n : scene.rootNode()->childNodes(false)) {
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
		if (!MOVE_SPEED) MOVE_SPEED = Max(scene.extent());

		auto keysDown = m_inputManager->keysDown();
		
		float moveMultiplier = 1.0;
		if (keysDown.count(Key_LeftShift)) {
			moveMultiplier = 2.0;
		}
		
		if (keysDown.count(Key_W) || mouseButtonsDown.count(MouseButton_4)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camForward;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		else if (keysDown.count(Key_S)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * -camForward;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		
		if (keysDown.count(Key_A)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * -camRight;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		else if (keysDown.count(Key_D)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camRight;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		
		if (keysDown.count(Key_Space)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camUp;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
	}
}

void Test::didSimulatePhysicsCallback(Scene& scene, float time) {
	m_logger->trace("didSimulatePhysicsCallback()");
}

void Test::windowWillRenderCallback(Scene& scene, float time) {
	
}

void Test::windowDidRenderCallback(Scene& scene, float time) {
	
}


