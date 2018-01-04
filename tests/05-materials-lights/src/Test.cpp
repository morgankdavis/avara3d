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
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/transform.hpp>

#include "ae.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;
using namespace std::placeholders;
using namespace glm;


#define USE_HIGH_DPI            true
#define WINDOW_WIDTH			800
#define WINDOW_HEIGHT			600
#define FULLSCREEN 				false


/***************************************************************************************
     MARK:   Static
 **************************************************************************************/

void SetAllFilterModes(FilterMode mode, Scene& scene) {

	cout << "SetAllFilterModes: " << mode << endl;
	
	for (auto node : scene.rootNode()->allChildNodes()) {
		
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
	
	for (auto node : scene.rootNode()->allChildNodes()) {
		
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
     MARK:   Public
 **************************************************************************************/

int Test::run(const vector<string>& args) {
	cout << "Test::run()\n" << endl;
	
	auto window = Window(FULLSCREEN, WINDOW_WIDTH, WINDOW_HEIGHT, USE_HIGH_DPI);
	window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
	window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
	window.enableCursor(false);
	window.vSyncEnabled(false);
	//window.antialiasingMode(AntialiasingMode_None);
	
	auto scene = make_shared<Scene>();

	auto siameseScene = TestSceneNamed("siamese");
	auto siameseNode = siameseScene->rootNode()->childNode("Siamese", true);
	siameseNode->scale(siameseNode->scale() * 0.070f);
	siameseNode->position(vec3(-13.5, -64.5, 0));
	//siameseNode->rotation(vec4(0.0f, 1.0f, 0.0f, radians(-15.0f)));
	scene->rootNode()->addChildNode(siameseNode);
//	for (auto m : siameseNode->geometry()->materials()) {
//		m->fillMode(MaterialFillMode_Line);
//	}

	auto islandScene = TestSceneNamed("Island", "obj");
	auto islandNode = islandScene->rootNode()->allChildNodes()[0];
	islandNode->position(vec3(0.0f, -150.0f, 0.0f));
	scene->rootNode()->addChildNode(islandNode);

	auto palletScene = TestSceneNamed("Pallet_rot");
	auto palletNode = palletScene->rootNode()->allChildNodes()[2];
	//palletNode->position(vec3(-35.0f, 35.0f, 0.0f));
	palletNode->position(vec3(-63.25f, -64.5f, -2.0f));
	palletNode->scale(palletNode->scale() * 20.0f);
	//palletNode->eulerAngles(vec3(radians(-90.0f), radians(40.0f), radians(0.0f)));
	
	
//	auto rotateX = quat_cast(rotate(mat4(1.0f), radians(90.0f), vec3(1.0f, 0.0f, 0.0f)));
//	auto rotateY = quat_cast(rotate(mat4(1.0f), radians(-130.0f), vec3(0.0f, 1.0f, 0.0f)));
//	auto rotateZ = quat_cast(rotate(mat4(1.0f), radians(40.0f), vec3(0.0f, 0.0f, 1.0f)));
//	quat orientation = quat(1.0, 0.0, 0.0, 0.0);
//	//palletNode->orientation(orientation * rotateZ * rotateY * rotateX);
//	palletNode->orientation(orientation * rotateY * rotateX * rotateZ);
	
	auto palletSpecularProperty = make_shared<MaterialProperty>(make_shared<Color>(Color::DarkGray()));
	palletNode->geometry()->firstMaterial()->specular(palletSpecularProperty);
	scene->rootNode()->addChildNode(palletNode);
	
	auto tunaScene = TestSceneNamed("tuna_rot");
	tunaScene->rootNode()->position(vec3(-7.5f, -72.0f, 40.0f));
	tunaScene->rootNode()->scale(tunaScene->rootNode()->scale() * 1.8f);
	//tunaScene->rootNode()->rotation(vec4(0.0f, 1.0f, 0.0f, radians(-90.0f)));
	scene->rootNode()->addChildNode(tunaScene->rootNode());

	auto palm1Scene = TestSceneNamed("palm1", "obj");
	palm1Scene->rootNode()->position(vec3(0.0f, -72.0f, 0.0f));
	palm1Scene->rootNode()->scale(palm1Scene->rootNode()->scale() * 2.5f);
	palm1Scene->rootNode()->rotation(vec4(0.0f, 1.0f, 0.0f, radians(-5.0f)));
	scene->rootNode()->addChildNode(palm1Scene->rootNode());
	
	for (auto n : palm1Scene->rootNode()->allChildNodes()) {
		if (n->geometry()) {
			for (auto m : n->geometry()->materials()) {
				m->doubleSided(true);
			}
		}
	}

//	auto background = make_shared<MaterialProperty>(TestCubeNamed("nebula1_blue", "png"));
	auto background = make_shared<MaterialProperty>(TestCubeNamed("sky1", "png"));
//	auto background = make_shared<MaterialProperty>(TestCubeNamed("yokohama", "jpg"));
//	auto background = make_shared<MaterialProperty>(TestCubeNamed("fishermansBastion", "jpg"));
	scene->background(background);
//	auto backgroundColor = make_shared<Color>(Color::Navy());
//	auto background = make_shared<MaterialProperty>(backgroundColor);
//	scene->background(background);


	auto ambientLight = make_shared<Light>(LightType_Ambient, make_shared<Color>(0.1, 0.1, 0.1, 1.0));
//	auto ambientLight = make_shared<Light>(LightType_Ambient, make_shared<Color>(0.2, 0.2, 0.2, 1.0));
//	auto ambientLight = make_shared<Light>(LightType_Ambient, make_shared<Color>(1.0, 1.0, 1.0, 1.0));
	auto ambientLightNode = make_shared<Node>(ambientLight);
	m_ambientLightNode = ambientLightNode;
	scene->rootNode()->addChildNode(ambientLightNode);


	auto pointLight = make_shared<Light>(LightType_Point, make_shared<Color>(Color::White()));
	pointLight->attenuationFactor(0.00015);
	auto pointLightNode = make_shared<Node>(pointLight);
	pointLightNode->position(vec3(50.0, 50.0, 50.0));
	scene->rootNode()->addChildNode(pointLightNode);

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
//		auto light = make_shared<Light>(LightType_Point);
//		light->attenuationFactor(0.0001);
//		auto lightNode = make_shared<Node>(light);
//		int randX = Random(-150, 150);
//		int randY = Random(-150, 150);
//		int randZ = Random(-150, 150);
//		lightNode->position(vec3(randX, randY, randZ));
//		auto color = make_shared<Color>(colors[Random(4, colors.size()-1-4)]);
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
//		scene->rootNode()->addChildNode(lightNode);
//	}
	
	
	scene->fogStartDistance(500.0);
	scene->fogEndDistance(5000.0);
	scene->fogDensityExponent(1.0);
	scene->fogColor(make_shared<Color>(Color::LightGray()));

	
	window.scene(scene);
	m_inputManager = window.inputManager();
	window.display();
	
	return 0;
}

void Test::windowWillUpdateCallback(Scene& scene, float deltaSeconds) {

	static float totalSeconds = 0;
	totalSeconds += deltaSeconds;

	// get input

	auto keysDown = m_inputManager->keysDown();
	if (keysDown.count(Key_Escape)) {
		exit(0);
	}
	
	if 		(keysDown.count(Key_1))	SetAllFilterModes(FilterMode_Nearest, scene);
	else if (keysDown.count(Key_2))	SetAllFilterModes(FilterMode_Linear, scene);
	else if (keysDown.count(Key_3))	SetAllFilterModes(FilterMode_NearestMipmapNearest, scene);
	else if (keysDown.count(Key_4))	SetAllFilterModes(FilterMode_NearestMipmapLinear, scene);
	else if (keysDown.count(Key_5))	SetAllFilterModes(FilterMode_LinearMipmapNearest, scene);
	else if (keysDown.count(Key_6))	SetAllFilterModes(FilterMode_LinearMipmapLinear, scene);
	
	if (keysDown.count(Key_Up)) 		SetAllMaxAnisotropy(16, scene);
	else if (keysDown.count(Key_Down)) 	SetAllMaxAnisotropy(1, scene);


	if 		(keysDown.count(Key_F10)) 	m_ambientLightNode->light()->color(make_shared<Color>(0.1, 0.1, 0.1, 1.0));
	else if (keysDown.count(Key_F11)) 	m_ambientLightNode->light()->color(make_shared<Color>(0.2, 0.2, 0.2, 1.0));
	else if (keysDown.count(Key_F12)) 	m_ambientLightNode->light()->color(make_shared<Color>(0.3, 0.3, 0.3, 1.0));


	if 		(keysDown.count(Key_F1)) 	m_pointLightNode->light()->attenuationFactor(0.0005);
	else if (keysDown.count(Key_F2)) 	m_pointLightNode->light()->attenuationFactor(0.00015);
	else if (keysDown.count(Key_F3)) 	m_pointLightNode->light()->attenuationFactor(0.00005);

	
	vec2 mousePositionDelta = m_inputManager->mousePositionDelta();

	// move camera

	const static float mouseSensitivity = (1.0f / 0.5f);


	if (!m_cameraNode) {
		for (auto n : scene.rootNode()->immediateChildNodes()) {
			if (n->camera()) {
				m_cameraNode = n;
				break;
			}
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

//		const static float MOVE_SPEED = 5.0f; // units/sec
		static float MOVE_SPEED = 0;
		if (!MOVE_SPEED) MOVE_SPEED = Max(scene.extent());

		if(keysDown.count(Key_W)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(Key_S)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}

		if(keysDown.count(Key_A)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(Key_D)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}

		if(keysDown.count(Key_Space)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
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

void Test::windowDidUpdateCallback(Scene& scene, float deltaSeconds) {

}
