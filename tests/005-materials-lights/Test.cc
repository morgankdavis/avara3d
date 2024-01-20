//
//  Example.cc
//	avara-engine
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Test.h"

#include <iostream>

#include <glm/glm.hpp>

#include "ae/utilities/Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;
using namespace std::placeholders;
using namespace test;


constexpr bool					USE_HIGH_DPI =			true;
constexpr unsigned				WINDOW_WIDTH =			1024;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr ANTIALIASING_MODE		ANTIALIAS_MODE =		ANTIALIASING_MODE::MSAA_4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr bool 					ORTHO_CAMERA =			false;
constexpr float					MOUSE_SENSITIVITY =		0.5;


/***************************************************************************************
	Static Prototypes
 ***************************************************************************************/

static void SetAllFilterModes(FILTER_MODE mode, Scene& scene);
static void SetAllMaxAnisotropy(float anisotropy, Scene& scene);

/***************************************************************************************
	Public
 ***************************************************************************************/

int Test::run(const vector<string>& args) {

	//AE_INIT();

	_logger = make_shared<Logger>("example", Logger::MainLogger()->sinks());
	_logger->level(LOG_LEVEL::DEBUG);
	Logger::MainLogger()->level(LOG_LEVEL::DEBUG);
	
	LOG_I(_logger, "");

	auto window = make_shared<Window>(RENDER_API::OPENGL,
									  FULLSCREEN,
									  WINDOW_WIDTH,
									  WINDOW_HEIGHT,
									  USE_HIGH_DPI,
									  ANTIALIAS_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_shared<VisualWorld>(window);
	visualWorld->fogStartDistance(500.0);
	visualWorld->fogEndDistance(5000.0);
	visualWorld->fogDensityExponent(1.0);
	visualWorld->fogColor(Color::LightGray());
	visualWorld->willRender(bind(&Test::willRenderCallback, this, _1, _2));
	visualWorld->didRender(bind(&Test::didRenderCallback, this, _1, _2));
	visualWorld->background(make_shared<MaterialProperty>(CubeImageNamed("nebula1_blue", "png")));

	auto inputManager = make_shared<WindowInputManager>(window);

	auto scene = make_shared<Scene>(visualWorld, nullptr, inputManager);
	scene->debugOptions(DEBUG_OPTIONS::SHOW_STATS_OVERLAY);
	scene->update(bind(&Test::updateCallback, this, _1, _2));

	auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, make_shared<Color>(0.2f, 0.2, 0.2, 1.0));
	ambientLight->name("ambient");
	auto ambientLightNode = Node::LightNode(ambientLight);
	_ambientLightNode = ambientLightNode;
	scene->rootNode()->addChild(ambientLightNode);

	auto pointLight = make_shared<Light>(LIGHT_TYPE::POINT, Color::White());
	pointLight->name("point");
	pointLight->attenuationFactor(0.00005);
	auto pointLightNode = Node::LightNode(pointLight);
	pointLightNode->position(vec3(50.0, 50.0, 50.0));
	scene->rootNode()->addChild(pointLightNode);
	pointLightNode->position(vec3(0.0, 0.0, 0.0));
	_pointLightNode = pointLightNode;
	auto materialProperty = make_shared<MaterialProperty>(pointLight->color());
	auto material = make_shared<Material>();
	material->name("LIGHT material");
	material->emissive(materialProperty);
	auto geometry = make_shared<Sphere>(3.5, 16);
	geometry->addMaterial(material);
	pointLightNode->geometry(geometry);

	if (ORTHO_CAMERA) {
		auto orthoCameraNode = Node::CameraNode(
				make_shared<OrthographicCamera>("Ortho camera", (AABB){vec3{0, 0, 0},
																	   vec3{100, 100, 100}}));
		scene->rootNode()->addChild(orthoCameraNode);
	}

	auto siameseScene = SceneNamed("siamese/siamese");
	auto siameseNode = siameseScene->rootNode()->childNamed("Siamese", true);
	_siameseNode = siameseNode;
	siameseNode->scale(siameseNode->scale() * 0.070f);
	siameseNode->position(vec3(-13.5, -64.5, 0));
	scene->rootNode()->addChild(siameseNode);
	siameseScene = nullptr;
	siameseNode = nullptr;

	auto islandScene = SceneNamed("island/Island", "obj");
	//auto islandNode = islandScene->rootNode()->children(true)[0];
	auto islandNode = islandScene->rootNode();
	islandNode->position(vec3(0.0f, -150.0f, 0.0f));
	scene->rootNode()->addChild(islandNode);

	auto palletScene = SceneNamed("pallet_rot/Pallet_rot");
//	auto palletNode = palletScene->rootNode()->children(true)[1];
	//auto palletNode = palletScene->rootNode()->children(true)[0];
	auto palletNode = palletScene->rootNode()->childNamed("Pallet", true);
	_palletNode = palletNode;
	palletNode->position(vec3(-63.25f, -64.5f, -2.0f));
	palletNode->scale(palletNode->scale() * 20.0f);
	auto palletSpecularProperty = make_shared<MaterialProperty>(Color::DarkGray());
	palletNode->geometry()->firstMaterial()->specular(palletSpecularProperty);
	scene->rootNode()->addChild(palletNode);
	
	auto tunaScene = SceneNamed("tuna_rot/tuna_rot");
	tunaScene->rootNode()->position(vec3(-7.5f, -72.0f, 40.0f));
	tunaScene->rootNode()->scale(tunaScene->rootNode()->scale() * 1.8f);
	scene->rootNode()->addChild(tunaScene->rootNode());

	auto palm1Scene = SceneNamed("palm1/palm1", "obj");
	_palmsNode = palm1Scene->rootNode();
	palm1Scene->rootNode()->position(vec3(0.0f, -72.0f, 0.0f));
	palm1Scene->rootNode()->scale(palm1Scene->rootNode()->scale() * 2.5f);
	palm1Scene->rootNode()->rotation({0.0f, 1.0f, 0.0f}, radians(-5.0f));
	scene->rootNode()->addChild(palm1Scene->rootNode());
	
	for (auto n : palm1Scene->rootNode()->children(true)) {
		if (n->geometry()) {
			for (auto m : n->geometry()->materials()) {
				m->doubleSided(true);
			}
		}
	}

	// random lights
	const int NUM_RANDOM_LIGHTS = 100;
	for (int l=0; l<NUM_RANDOM_LIGHTS; ++l) {
		auto light = make_shared<Light>(LIGHT_TYPE::POINT);
		light->attenuationFactor(0.0001);
		auto lightNode = Node::LightNode(light);
		int randX = Uniform(-150, 150);
		int randY = Uniform(-150, 150);
		int randZ = Uniform(-150, 150);
		lightNode->position(vec3(randX, randY, randZ));
		auto color = Color::Random();
		light->color(color);
		cout << "Adding random light with position: "
		<< lightNode->position()<< ", color: " << *light->color() << endl;

		auto geometry = make_shared<Sphere>(3.5, 16);

		auto materialProperty = make_shared<MaterialProperty>(color);
		auto material = make_shared<Material>();
		material->emissive(materialProperty);
		geometry->addMaterial(material);
		lightNode->geometry(geometry);

		scene->rootNode()->addChild(lightNode);
	}

	window->open();
	scene->run();
	
	return 0;
}

/***************************************************************************************
	Scene Callbacks
 ***************************************************************************************/

void Test::updateCallback(Scene& scene, double time) {
	LOG_T(_logger, "scene: {:p}, time: {}", (void*)&scene, time);
	
	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;

	auto window = static_pointer_cast<Window>(scene.visualWorld()->renderContext());

	// get input
	
	auto keysPressed = scene.inputManager()->keysPressed();
	
	if (keysPressed.count(KEY::ESCAPE)) {
		window->close();
	}
	
	if (keysPressed.count(KEY::T)) {
		LOG_I(_logger, "TREE:\n{}", StringFromTree(*(scene.rootNode())));
	}
	
	if 		(keysPressed.count(KEY::ONE))	SetAllFilterModes(FILTER_MODE::NEAREST, scene);
	else if (keysPressed.count(KEY::TWO))	SetAllFilterModes(FILTER_MODE::LINEAR, scene);
	else if (keysPressed.count(KEY::THREE))	SetAllFilterModes(FILTER_MODE::NEAREST_MIPMAP_NEAREST, scene);
	else if (keysPressed.count(KEY::FOUR))	SetAllFilterModes(FILTER_MODE::NEAREST_MIPMAP_LINEAR, scene);
	else if (keysPressed.count(KEY::FIVE))	SetAllFilterModes(FILTER_MODE::LINEAR_MIPMAP_NEAREST, scene);
	else if (keysPressed.count(KEY::SIX))	SetAllFilterModes(FILTER_MODE::LINEAR_MIPMAP_LINEAR, scene);
	
	if 		(keysPressed.count(KEY::LEFT_BRACKET))	SetAllMaxAnisotropy(1, scene);
	else if (keysPressed.count(KEY::RIGHT_BRACKET))	SetAllMaxAnisotropy(16, scene);
	
	if 		(keysPressed.count(KEY::F10)) 	_ambientLightNode->light()->color(make_shared<Color>(0.1f, 0.1, 0.1, 1.0));
	else if (keysPressed.count(KEY::F11)) 	_ambientLightNode->light()->color(make_shared<Color>(0.2f, 0.2, 0.2, 1.0));
	else if (keysPressed.count(KEY::F12)) 	_ambientLightNode->light()->color(make_shared<Color>(0.3f, 0.3, 0.3, 1.0));
	
	if 		(keysPressed.count(KEY::F1)) 	_pointLightNode->light()->attenuationFactor(0.0005);
	else if (keysPressed.count(KEY::F2)) 	_pointLightNode->light()->attenuationFactor(0.00015);
	else if (keysPressed.count(KEY::F3)) 	_pointLightNode->light()->attenuationFactor(0.00005);

	if (keysPressed.count(KEY::F)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
	}

	if (keysPressed.count(KEY::B)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
	}

	if (keysPressed.count(KEY::I)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
	}
	
	if (keysPressed.count(KEY::V)) {
		window->vSyncEnabled(!(window->vSyncEnabled()));
	}
	
	if (keysPressed.count(KEY::BACKSLASH)) {
		SaveSnapshot(*window);
	}
	
	if (keysPressed.count(KEY::R)) {
		if (!window->recordingGIF()) {
			StartGIFRecording(*window, 320, 8);
		}
		else {
			StopGIFRecording(*window);
		}
	}
	
	if (keysPressed.count(KEY::SLASH)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}
	
	if (keysPressed.count(KEY::U)) {
		_siameseNode->removeFromParent();
	}

	if (keysPressed.count(KEY::O)) {
		_siameseNode = nullptr;
	}
	
//	if (keysPressed.count(Key_M)) {
//		unsigned total = 0;
//		unsigned used = 0;
//		_window->getVRAMStats(total, used);
//	}
	
//	if (keysPressed.count(KEY::DEL)) {
//		for (auto n : _window->scene()->rootNode()->children(true)) {
//			n->geometry(nullptr);
//		}
//	}

	
	if (keysPressed.count(KEY::FORWARD_DELETE)) {
		_palletNode->geometry(nullptr);
	}
	
	if (keysPressed.count(KEY::END)) {
		_siameseNode->geometry(nullptr);
	}
	
	if (keysPressed.count(KEY::PAGE_DOWN)) {
		for (auto& n : _palmsNode->children(true)) {
			n->geometry(nullptr);
		}
	}
	
//	for (auto& key : keysPressed) {
//		LOG_D(_logger, "KEY: {}", static_cast<int>(key));
//	}
	
	if (keysPressed.count(KEY::HOME)) {
		auto squirrelImage = ImageNamed("squirrel2");
		_siameseNode->geometry()->firstMaterial()->diffuse()->contents(squirrelImage);
	}
	
	if (keysPressed.count(KEY::PAGE_UP)) {
		_siameseNode->geometry(_palletNode->geometry());
	}
	
	if (keysPressed.count(KEY::ZERO)) {
		auto teapot = SceneNamed("teapot", "dae");
		_siameseNode->geometry(teapot->rootNode()->children(false)[1]->geometry());
	}

	if (window->cursorCaptured()) {
		
		vec2 mousePositionDelta = scene.inputManager()->mousePositionDelta();
		
		// move camera

		auto pov = scene.visualWorld()->pointOfView();
		if (pov) {

			vec2 mouseScrollWheelDelta = scene.inputManager()->mouseScrollWheelDelta();
			if (mouseScrollWheelDelta.y) {

				static const float FOV_SPEED = 2.5; // degrees/roll

				shared_ptr<PerspectiveCamera> camera = static_pointer_cast<PerspectiveCamera>(pov->camera());
				auto fov = camera->fov();
				fov += mouseScrollWheelDelta.y * -radians(FOV_SPEED);
				camera->fov(fov);
			}

			//cout << "Camera distance: " << length(_cameraNode->position()) << endl;
			
			// look
			
			vec3 camForward = pov->worldForward();
			vec3 camRight = pov->worldRight();
			vec3 camUp = pov->worldUp();

			static const float MOUSE_SPEED_SCALAR = .002;
			static const float MOUSE_SPEED = MOUSE_SENSITIVITY * MOUSE_SPEED_SCALAR;

			float deltaRotX = atan(MOUSE_SPEED * mousePositionDelta.x);
			float deltaRotY = atan(MOUSE_SPEED * mousePositionDelta.y);
			
			vec3 angles = pov->eulerAngles();
			pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));
			
			// move
			
			auto keysDown = scene.inputManager()->keysDown();

			static float MOVE_SPEED = 0;
			if (!MOVE_SPEED) MOVE_SPEED = Max(scene.rootNode()->extent());

			float moveMultiplier = 1.0;
			if (keysDown.count(KEY::LEFT_CONTROL)) {
				moveMultiplier = 2.0;
			}
			
			if(keysDown.count(KEY::W)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
				pov->position(pov->position() + positionDelta);
			}
			else if(keysDown.count(KEY::S)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
				pov->position(pov->position() + positionDelta);
			}
			
			if(keysDown.count(KEY::A)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
				pov->position(pov->position() + positionDelta);
			}
			else if(keysDown.count(KEY::D)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(KEY::SPACE)) {
				float direction = 1;
				if (keysDown.count(KEY::LEFT_SHIFT)) {
					direction = -1;
				}
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camUp;
				pov->position(pov->position() + positionDelta * direction);
			}
		}
	}
	
	// move the light
	
	if (_pointLightNode) {

		auto center = vec3(0, -75, 0);

		static float radiusX = 100.0;
		static float radiusY = 100.0;

		static float rotationSpeed = radians(30.0); // deg/secs
		static float angle = 0;
		angle += rotationSpeed * deltaSeconds;

		float x = sin(angle) * radiusX;
		float y = cos(angle) * radiusY;

		_pointLightNode->position(center + vec3(x, y, -x));
	}
}

/***************************************************************************************
	VisualWorld Callbacks
 ***************************************************************************************/

void Test::willRenderCallback(VisualWorld& world, double time) {
	LOG_T(_logger, "world: {:p}, time: {}", (void*)&world, time);
}

void Test::didRenderCallback(VisualWorld& world, double time) {
	LOG_T(_logger, "world: {:p}, time: {}", (void*)&world, time);
}

/***************************************************************************************
	Static
 ***************************************************************************************/

void SetAllFilterModes(FILTER_MODE mode, Scene& scene) {

	cout << "SetAllFilterModes: " << (unsigned)mode << endl;
	
	for (auto node : scene.rootNode()->children(true)) {
		
		auto geometry = node->geometry();
		if (geometry) {
			
			for (auto material : geometry->materials()) {
				if (material->diffuse()) {
					material->diffuse()->minificationFilter(mode);
					material->diffuse()->magnificationFilter(mode);
				}
				if (material->specular()) {
					material->specular()->minificationFilter(mode);
					material->specular()->magnificationFilter(mode);
				}
			}
		}
	}
}

void SetAllMaxAnisotropy(float anisotropy, Scene& scene) {
	
	cout << "SetAllMaxAnisotropy: " << anisotropy << endl;
	
	for (auto node : scene.rootNode()->children(true)) {
		
		auto geometry = node->geometry();
		if (geometry) {
			
			for (auto material : geometry->materials()) {
				if (material->diffuse()) material->diffuse()->maxAnisotropy(anisotropy);
				if (material->specular()) material->specular()->maxAnisotropy(anisotropy);
			}
		}
	}
}
