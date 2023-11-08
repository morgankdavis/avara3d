//
//  Example.cc
//	avara-engine
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Example.h"

#include <iostream>

#include <glm/glm.hpp>

#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace example;
using namespace std;
using namespace std::placeholders;
using namespace glm;


constexpr bool					USE_HIGH_DPI =			true;
constexpr unsigned				WINDOW_WIDTH =			1024;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr ANTIALIASING_MODE		ANTIALIAS_MODE =		ANTIALIASING_MODE::NONE;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr bool 					ORTHO_CAMERA =			false;


/***************************************************************************************
	Static Prototypes
 ***************************************************************************************/

static void SetAllFilterModes(FILTER_MODE mode, Scene& scene);
static void SetAllMaxAnisotropy(float anisotropy, Scene& scene);

/***************************************************************************************
	Public
 ***************************************************************************************/

int Example::run(const vector<string>& args) {
	AE_INIT();

	_logger = make_shared<Logger>("example", Logger::MainLogger()->sinks());
	_logger->level(LOG_LEVEL::TRACE_);
	Logger::MainLogger()->level(LOG_LEVEL::TRACE_);
	
	LOG_I(_logger, "Example::run()");

	_window = make_shared<Window>(FULLSCREEN,
								  WINDOW_WIDTH, WINDOW_HEIGHT,
								  USE_HIGH_DPI,
								  ANTIALIAS_MODE,
								  RENDER_API::OPENGL);
	_window->updateCallback(bind(&Example::updateCallback, this, _1, _2));
	_window->willRenderCallback(bind(&Example::willRenderCallback, this, _1, _2));
	_window->didRenderCallback(bind(&Example::didRenderCallback, this, _1, _2));
	_window->enableVSync(ENABLE_VSYNC);
	_window->captureCursor(CAPTURE_CURSOR);
	_window->debugOptions(DEBUG_OPTIONS::SHOW_STATS_OVERLAY);

	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));

	if (ORTHO_CAMERA) {
		auto orthoCameraNode = Node::CameraNode(
				make_shared<OrthographicCamera>("Ortho camera", (AABB){vec3{0, 0, 0},
																	   vec3{100, 100, 100}}));
		scene->rootNode()->addChild(orthoCameraNode);
	}

	auto siameseScene = SceneNamed("siamese/siamese");
	auto siameseNode = siameseScene->rootNode()->child("Siamese", true);
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
	auto palletNode = palletScene->rootNode()->children(true)[0];
	palletNode->name("Pallet node");
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
	palm1Scene->rootNode()->rotation(vec4(0.0f, 1.0f, 0.0f, radians(-5.0f)));
	scene->rootNode()->addChild(palm1Scene->rootNode());
	
	for (auto n : palm1Scene->rootNode()->children(true)) {
		if (n->geometry()) {
			for (auto m : n->geometry()->materials()) {
				m->doubleSided(true);
			}
		}
	}
	
	auto background = make_shared<MaterialProperty>(CubeImageNamed("nebula1_blue", "png"));
//	auto background = make_shared<MaterialProperty>(Color::Navy());
	scene->background(background);


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
	
	
//	// random lights
//	const int NUM_RANDOM_LIGHTS = 100;
//	auto colors = Color::Rainbow();
//	for (int l=0; l<NUM_RANDOM_LIGHTS; ++l) {
//		auto light = make_shared<Light>(LIGHT_TYPE::POINT);
//		light->attenuationFactor(0.0001);
//		//auto lightNode = make_shared<Node>(light);
////		auto lightNode = make_shared<Node>("Light");
////		lightNode->light(light);
//		auto lightNode = Node::LightNode(light);
//		int randX = Random(-150, 150);
//		int randY = Random(-150, 150);
//		int randZ = Random(-150, 150);
//		lightNode->position(vec3(randX, randY, randZ));
//		auto color = colors[Random(4, colors.size()-1-4)];
//		light->color(color);
//		cout << "Adding random light with position: "
//		<< lightNode->position()<< ", color: " << *light->color() << endl;
//
//		auto geometry = make_shared<Sphere>(3.5, 16);
//
//		auto materialProperty = make_shared<MaterialProperty>(color);
//		auto material = make_shared<Material>();
//		material->emissive(materialProperty);
//		geometry->addMaterial(material);
//		lightNode->geometry(geometry);
//
//		scene->rootNode()->addChild(lightNode);
//	}
	
	
	scene->fogStartDistance(500.0);
	scene->fogEndDistance(5000.0);
	scene->fogDensityExponent(1.0);
	scene->fogColor(Color::LightGray());
	

	_window->scene(scene);
	_inputManager = _window->inputManager();
	_window->display();
	
	return 0;
}

/***************************************************************************************
	RenderContext Callbacks
 ***************************************************************************************/

void Example::updateCallback(RenderContext& renderContext, float time) {
	
	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;

	if (!_cameraNode) {
		for (auto n : renderContext.scene()->rootNode()->children(false)) {
			if (n->camera()) {
				_cameraNode = n;
				break;
			}
		}
	}
	
	// get input
	
	auto keysPressed = _inputManager->keysPressed();
	
	if (keysPressed.count(KEY::ESCAPE)) {
		_window->setShouldClose();
	}
	
	if (keysPressed.count(KEY::T)) {
		LOG_I(_logger, "TREE:\n{}", StringFromTree(*(renderContext.scene()->rootNode())));
	}
	
	if 		(keysPressed.count(KEY::ONE))	SetAllFilterModes(FILTER_MODE::NEAREST, *(renderContext.scene()));
	else if (keysPressed.count(KEY::TWO))	SetAllFilterModes(FILTER_MODE::LINEAR, *(renderContext.scene()));
	else if (keysPressed.count(KEY::THREE))	SetAllFilterModes(FILTER_MODE::NEAREST_MIPMAP_NEAREST, *(renderContext.scene()));
	else if (keysPressed.count(KEY::FOUR))	SetAllFilterModes(FILTER_MODE::NEAREST_MIPMAP_LINEAR, *(renderContext.scene()));
	else if (keysPressed.count(KEY::FIVE))	SetAllFilterModes(FILTER_MODE::LINEAR_MIPMAP_NEAREST, *(renderContext.scene()));
	else if (keysPressed.count(KEY::SIX))	SetAllFilterModes(FILTER_MODE::LINEAR_MIPMAP_LINEAR, *(renderContext.scene()));
	
	if 		(keysPressed.count(KEY::LEFT_BRACKET))	SetAllMaxAnisotropy(1, *(renderContext.scene()));
	else if (keysPressed.count(KEY::RIGHT_BRACKET))	SetAllMaxAnisotropy(16, *(renderContext.scene()));
	
	if 		(keysPressed.count(KEY::F10)) 	_ambientLightNode->light()->color(make_shared<Color>(0.1f, 0.1, 0.1, 1.0));
	else if (keysPressed.count(KEY::F11)) 	_ambientLightNode->light()->color(make_shared<Color>(0.2f, 0.2, 0.2, 1.0));
	else if (keysPressed.count(KEY::F12)) 	_ambientLightNode->light()->color(make_shared<Color>(0.3f, 0.3, 0.3, 1.0));
	
	if 		(keysPressed.count(KEY::F1)) 	_pointLightNode->light()->attenuationFactor(0.0005);
	else if (keysPressed.count(KEY::F2)) 	_pointLightNode->light()->attenuationFactor(0.00015);
	else if (keysPressed.count(KEY::F3)) 	_pointLightNode->light()->attenuationFactor(0.00005);

	if (keysPressed.count(KEY::F)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
	}
	if (keysPressed.count(KEY::B)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
	}
	if (keysPressed.count(KEY::I)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
	}
	
	if (keysPressed.count(KEY::V)) {
		_window->enableVSync(!(_window->vSyncEnabled()));
	}
	
	if (keysPressed.count(KEY::BACKSLASH)) {
		SaveSnapshot(*_window);
	}
	
	if (keysPressed.count(KEY::R)) {
		if (!_window->recordingGIF()) {
			StartGIFRecording(*_window, 240, 8);
		}
		else {
			StopGIFRecording(*_window);
		}
	}
	
	if (keysPressed.count(KEY::SLASH)) {
		_window->captureCursor(!(_window->cursorCaptured()));
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

	if (_window->cursorCaptured()) {
		
		vec2 mousePositionDelta = _inputManager->mousePositionDelta();
		
		// move camera
		
		if (_cameraNode) {

			const static float mouseSensitivity = (1.0f / 0.5f);

			vec2 mouseScrollWheelDelta = _inputManager->mouseScrollWheelDelta();
			if (mouseScrollWheelDelta.y) {
				AE_LOG_I("mouseScrollWheelDelta");
				static float FOV_SPEED = 2.5; // degrees/roll
				if (_cameraNode) {
					AE_LOG_I("_cameraNode");
					shared_ptr<PerspectiveCamera> camera = static_pointer_cast<PerspectiveCamera>(camera);
					auto fov = camera->fov();
					AE_LOG_I("oldFOV: {}", fov);
					fov += mouseScrollWheelDelta.y * -radians(FOV_SPEED);
					AE_LOG_I("newFOV: {}", fov);
					camera->fov(fov);
				}
			}

			//cout << "Camera distance: " << length(_cameraNode->position()) << endl;
			
			// look
			
			vec3 camForward = _cameraNode->worldForward();
			vec3 camRight = _cameraNode->worldRight();
			vec3 camUp = _cameraNode->worldUp();
			
			float deltaRotX = atan(deltaSeconds * mousePositionDelta.x / mouseSensitivity);
			float deltaRotY = atan(deltaSeconds * mousePositionDelta.y / mouseSensitivity);
			
			vec3 angles = _cameraNode->eulerAngles();
			_cameraNode->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));
			
			// move
			
			auto keysDown = _inputManager->keysDown();
			
			//		const static float MOVE_SPEED = 5.0f; // units/sec
			static float MOVE_SPEED = 0;
			if (!MOVE_SPEED) MOVE_SPEED = Max(renderContext.scene()->extent());
			
			if(keysDown.count(KEY::W)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
				_cameraNode->position(_cameraNode->position() + positionDelta);
			}
			else if(keysDown.count(KEY::S)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
				_cameraNode->position(_cameraNode->position() + positionDelta);
			}
			
			if(keysDown.count(KEY::A)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
				_cameraNode->position(_cameraNode->position() + positionDelta);
			}
			else if(keysDown.count(KEY::D)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
				_cameraNode->position(_cameraNode->position() + positionDelta);
			}
			
			if(keysDown.count(KEY::SPACE)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
				_cameraNode->position(_cameraNode->position() + positionDelta);
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

void Example::willRenderCallback(RenderContext& renderContext, float time) {

}

void Example::didRenderCallback(RenderContext& renderContext, float time) {

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
