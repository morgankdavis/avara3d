//
//  Test.cpp
//	avara-engine
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
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


#define USE_HIGH_DPI            true
#define WINDOW_WIDTH			1024
#define WINDOW_HEIGHT			768
#define FULLSCREEN 				false
#define ANTIALIAS_MODE			ANTIALIASING_MODE::MSAA_4X


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

/***************************************************************************************
     Public
 ***************************************************************************************/

int Test::run(const vector<string>& args) {
	AE_INIT();
	
	Logger::Level(LOG_LEVEL::DEBUG);
	
	AE_LOG->info("Test::run()");

	auto renderer = make_shared<OpenGLRenderer>();
	m_window = make_shared<Window>(static_pointer_cast<Renderer>(renderer),
								   FULLSCREEN,
								   WINDOW_WIDTH, WINDOW_HEIGHT,
								   USE_HIGH_DPI, ANTIALIAS_MODE);
	m_window->updateCallback(bind(&Test::updateCallback, this, _1, _2));
	m_window->willRenderCallback(bind(&Test::willRenderCallback, this, _1, _2));
	m_window->didRenderCallback(bind(&Test::didRenderCallback, this, _1, _2));
	m_window->captureCursor(true);
	m_window->enableVSync(false);
	m_window->debugOptions(DEBUG_OPTIONS::SHOW_STATS_OVERLAY);

	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));
	
	
	
//	auto sphereGeo = make_shared<Sphere>(0.5, 24);
//	auto sphereNode = make_shared<Node>();
//	sphereGeo->name("sphere");
//	sphereNode->geometry(sphereGeo);
//	scene->rootNode()->addChild(sphereNode);
//	sphereNode->position({0.0f, 0.0f, 0.0f});
//	
//	auto gridImage = TestImageNamed("grid10_512");
//	auto sphereMaterialProperty = make_shared<MaterialProperty>(gridImage);
//	sphereMaterialProperty->wrapS(WRAP_MODE::REPEAT);
//	sphereMaterialProperty->wrapT(WRAP_MODE::REPEAT);
//	auto sphereMaterial = make_shared<Material>(nullptr, sphereMaterialProperty, nullptr);
//	sphereNode->geometry()->addMaterial(sphereMaterial);
	
	

	auto siameseScene = TestSceneNamed("siamese");
	auto siameseNode = siameseScene->rootNode()->child("Siamese", true);
	m_siameseNode = siameseNode;
	siameseNode->scale(siameseNode->scale() * 0.070f);
	siameseNode->position(vec3(-13.5, -64.5, 0));
	scene->rootNode()->addChild(siameseNode);

	auto islandScene = TestSceneNamed("Island", "obj");
	auto islandNode = islandScene->rootNode()->children(true)[0];
	islandNode->position(vec3(0.0f, -150.0f, 0.0f));
	scene->rootNode()->addChild(islandNode);

	auto palletScene = TestSceneNamed("Pallet_rot");
	auto palletNode = palletScene->rootNode()->children(true)[1];
	palletNode->name("Pallet node");
	m_palletNode = palletNode;
	palletNode->position(vec3(-63.25f, -64.5f, -2.0f));
	palletNode->scale(palletNode->scale() * 20.0f);
	auto palletSpecularProperty = make_shared<MaterialProperty>(Color::DarkGray());
	palletNode->geometry()->firstMaterial()->specular(palletSpecularProperty);
	scene->rootNode()->addChild(palletNode);
	
	auto tunaScene = TestSceneNamed("tuna_rot");
	tunaScene->rootNode()->position(vec3(-7.5f, -72.0f, 40.0f));
	tunaScene->rootNode()->scale(tunaScene->rootNode()->scale() * 1.8f);
	scene->rootNode()->addChild(tunaScene->rootNode());

	auto palm1Scene = TestSceneNamed("palm1", "obj");
	m_palmsNode = palm1Scene->rootNode();
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
	
	auto background = make_shared<MaterialProperty>(TestCubeImageNamed("nebula1_blue", "png"));
//	auto background = make_shared<MaterialProperty>(Color::Navy());
	scene->background(background);


	auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, Color::DarkGray());
	auto ambientLightNode = Node::LightNode(ambientLight);
	m_ambientLightNode = ambientLightNode;
	scene->rootNode()->addChild(ambientLightNode);

	auto pointLight = make_shared<Light>(LIGHT_TYPE::POINT, Color::White());
	pointLight->attenuationFactor(0.00005);
	auto pointLightNode = Node::LightNode(pointLight);
	pointLightNode->position(vec3(50.0, 50.0, 50.0));
	scene->rootNode()->addChild(pointLightNode);
	pointLightNode->position(vec3(0.0, 0.0, 0.0));
	m_pointLightNode = pointLightNode;
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
	

	m_window->scene(scene);
	m_inputManager = m_window->inputManager();
	m_window->display();
	
	return 0;
}

/***************************************************************************************
     Window Callbacks
 ***************************************************************************************/

void Test::updateCallback(RenderContext& renderContext, float time) {
	
	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	// get input
	
	auto keysPressed = m_inputManager->keysPressed();
	
	if (keysPressed.count(KEY::ESCAPE)) {
		m_window->setShouldClose();
	}
	
	if 		(keysPressed.count(KEY::ONE))	SetAllFilterModes(FILTER_MODE::NEAREST, *(renderContext.scene()));
	else if (keysPressed.count(KEY::TWO))	SetAllFilterModes(FILTER_MODE::LINEAR, *(renderContext.scene()));
	else if (keysPressed.count(KEY::THREE))	SetAllFilterModes(FILTER_MODE::NEAREST_MIPMAP_NEAREST, *(renderContext.scene()));
	else if (keysPressed.count(KEY::FOUR))	SetAllFilterModes(FILTER_MODE::NEAREST_MIPMAP_LINEAR, *(renderContext.scene()));
	else if (keysPressed.count(KEY::FIVE))	SetAllFilterModes(FILTER_MODE::LINEAR_MIPMAP_NEAREST, *(renderContext.scene()));
	else if (keysPressed.count(KEY::SIX))	SetAllFilterModes(FILTER_MODE::LINEAR_MIPMAP_LINEAR, *(renderContext.scene()));
	
	if 		(keysPressed.count(KEY::LEFT_BRACKET))	SetAllMaxAnisotropy(1, *(renderContext.scene()));
	else if (keysPressed.count(KEY::RIGHT_BRACKET))	SetAllMaxAnisotropy(16, *(renderContext.scene()));
	
	if 		(keysPressed.count(KEY::F10)) 	m_ambientLightNode->light()->color(make_shared<Color>(0.1, 0.1, 0.1, 1.0));
	else if (keysPressed.count(KEY::F11)) 	m_ambientLightNode->light()->color(make_shared<Color>(0.2, 0.2, 0.2, 1.0));
	else if (keysPressed.count(KEY::F12)) 	m_ambientLightNode->light()->color(make_shared<Color>(0.3, 0.3, 0.3, 1.0));
	
	if 		(keysPressed.count(KEY::F1)) 	m_pointLightNode->light()->attenuationFactor(0.0005);
	else if (keysPressed.count(KEY::F2)) 	m_pointLightNode->light()->attenuationFactor(0.00015);
	else if (keysPressed.count(KEY::F3)) 	m_pointLightNode->light()->attenuationFactor(0.00005);

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
		m_window->enableVSync(!(m_window->vSyncEnabled()));
	}
	
	if (keysPressed.count(KEY::BACKSLASH)) {
		SaveSnapshot(*m_window);
	}
	
	if (keysPressed.count(KEY::R)) {
		if (!m_window->recordingGIF()) {
			StartGIFRecording(*m_window, 240, 8);
		}
		else {
			StopGIFRecording(*m_window);
		}
	}
	
	if (keysPressed.count(KEY::SLASH)) {
		m_window->captureCursor(!(m_window->cursorCaptured()));
	}
	
//	if (keysPressed.count(Key_M)) {
//		unsigned total = 0;
//		unsigned used = 0;
//		m_window->getVRAMStats(total, used);
//	}
	
//	if (keysPressed.count(KEY::DEL)) {
//		for (auto n : m_window->scene()->rootNode()->children(true)) {
//			n->geometry(nullptr);
//		}
//	}

	
	if (keysPressed.count(KEY::FORWARD_DELETE)) {
		m_palletNode->geometry(nullptr);
	}
	
	if (keysPressed.count(KEY::END)) {
		m_siameseNode->geometry(nullptr);
	}
	
	if (keysPressed.count(KEY::PAGE_DOWN)) {
		for (auto& n : m_palmsNode->children(true)) {
			n->geometry(nullptr);
		}
	}
	
//	for (auto& key : keysPressed) {
//		AE_LOG->debug("KEY: {}", static_cast<int>(key));
//	}
	
	if (keysPressed.count(KEY::HOME)) {
		auto squirrelImage = TestImageNamed("squirrel2");
		m_siameseNode->geometry()->firstMaterial()->diffuse()->contents(squirrelImage);
	}
	
	if (keysPressed.count(KEY::PAGE_UP)) {
		m_siameseNode->geometry(m_palletNode->geometry());
	}
	
	if (keysPressed.count(KEY::ZERO)) {
		auto teapot = TestSceneNamed("teapot", "obj");
		m_siameseNode->geometry(teapot->rootNode()->children(false)[0]->geometry());
	}
	
	

	if (m_window->cursorCaptured()) {
		
		vec2 mousePositionDelta = m_inputManager->mousePositionDelta();
		
		// move camera
		
		const static float mouseSensitivity = (1.0f / 0.5f);
		
		if (!m_cameraNode) {
			for (auto n : renderContext.scene()->rootNode()->children(false)) {
				if (n->camera()) {
					m_cameraNode = n;
					break;
				}
			}
		}
		
		vec2 mouseScrollWheelDelta = m_inputManager->mouseScrollWheelDelta();
		if (mouseScrollWheelDelta.y) {
			static float FOV_SPEED = 2.5; // degrees/roll
			if (m_cameraNode) {
				auto fov = m_cameraNode->camera()->fov();
				fov += mouseScrollWheelDelta.y * -radians(FOV_SPEED);
				m_cameraNode->camera()->fov(fov);
			}
		}
		
		if (m_cameraNode) {

			//cout << "Camera distance: " << length(m_cameraNode->position()) << endl;
			
			// look
			
			vec3 camForward = m_cameraNode->worldForward();
			vec3 camRight = m_cameraNode->worldRight();
			vec3 camUp = m_cameraNode->worldUp();
			
			float deltaRotX = atan(deltaSeconds * mousePositionDelta.x / mouseSensitivity);
			float deltaRotY = atan(deltaSeconds * mousePositionDelta.y / mouseSensitivity);
			
			vec3 angles = m_cameraNode->eulerAngles();
			m_cameraNode->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));
			
			// move
			
			auto keysDown = m_inputManager->keysDown();
			
			//		const static float MOVE_SPEED = 5.0f; // units/sec
			static float MOVE_SPEED = 0;
			if (!MOVE_SPEED) MOVE_SPEED = Max(renderContext.scene()->extent());
			
			if(keysDown.count(KEY::W)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
				m_cameraNode->position(m_cameraNode->position() + positionDelta);
			}
			else if(keysDown.count(KEY::S)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
				m_cameraNode->position(m_cameraNode->position() + positionDelta);
			}
			
			if(keysDown.count(KEY::A)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
				m_cameraNode->position(m_cameraNode->position() + positionDelta);
			}
			else if(keysDown.count(KEY::D)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
				m_cameraNode->position(m_cameraNode->position() + positionDelta);
			}
			
			if(keysDown.count(KEY::SPACE)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
				m_cameraNode->position(m_cameraNode->position() + positionDelta);
			}
		}
	}
	
	// move the light
	
	if (m_pointLightNode) {
		
		auto center = vec3(0, -75, 0);
		
		static float radiusX = 100.0;
		static float radiusY = 100.0;
		
		static float rotationSpeed = radians(30.0); // deg/secs
		static float angle = 0;
		angle += rotationSpeed * deltaSeconds;
		
		float x = sin(angle) * radiusX;
		float y = cos(angle) * radiusY;
		
		m_pointLightNode->position(center + vec3(x, y, -x));
	}
}

void Test::willRenderCallback(RenderContext& renderContext, float time) {

}

void Test::didRenderCallback(RenderContext& renderContext, float time) {

}
