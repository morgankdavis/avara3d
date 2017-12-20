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
//#include "InputManager.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;
using namespace std::placeholders;
using namespace glm;


#define FRAMEBUFFER_SCALE       1.0f
#define WINDOW_WIDTH			800
#define WINDOW_HEIGHT			600


/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

int Test::run(const vector<string>& args) {
	cout << "Test::run()\n" << endl;
	
	if (init() != 0) { cout << "Init error!" << endl; return -1; }

	auto scene = make_shared<Scene>();

	auto siameseScene = TestSceneNamed("siamese");
	auto siameseNode = siameseScene->rootNode()->childNode("Siamese", true);
	siameseNode->scale(siameseNode->scale() * 0.075f);
	siameseNode->position(vec3(-13.5, -64.0, 0));
	//siameseNode->rotation(vec4(0.0f, 1.0f, 0.0f, radians(-15.0f)));
	scene->rootNode()->addChildNode(siameseNode);
//	for (auto m : siameseNode->geometry()->materials()) {
//		m->fillMode(MaterialFillMode_Line);
//	}

//	auto testScene = TestSceneNamed("importTest");
//	scene->rootNode()->addChildNodes(testScene->rootNode()->allChildNodes());


//	auto planeGeo = make_shared<Plane>(100.0f, 100.0f);
//	auto planeNode = make_shared<Node>();
//	planeGeo->name("plane");
//	planeNode->geometry(planeGeo);
//	scene->rootNode()->addChildNode(planeNode);
//	//planeNode->scale(planeNode->scale() * 10.0f);
//	planeNode->rotation(vec4(-1.0f, 0.0f, 0.0f, radians(90.0f)));
//	planeNode->position(vec3(0.0f, -65.0f, 0.0f));
//	scene->rootNode()->addChildNode(planeNode);
	
//	auto tileImage = TestImageNamed("tiles_1_diff", "jpg");
//	auto planeMaterialProperty = make_shared<MaterialProperty>(tileImage);
//	auto planeMaterial = make_shared<Material>();
//	planeMaterial->diffuse(planeMaterialProperty);
//	planeGeo->materials().push_back(planeMaterial);
	
	
	
	auto islandScene = TestSceneNamed("Island", "obj");
	auto islandNode = islandScene->rootNode()->allChildNodes()[0];
	islandNode->position(vec3(0.0f, -150.0f, 0.0f));
	scene->rootNode()->addChildNode(islandNode);
	



	
	
//	auto siameseBadTextureScene = TestSceneNamed("siamese_badTexture");
//	auto siameseBadTextureNode = siameseBadTextureScene->rootNode()->childNode("Siamese", true);
//	siameseBadTextureNode->scale(siameseBadTextureNode->scale() * 0.15f);
//	siameseBadTextureNode->position(vec3(25.0, 0, 0));
////	scene->rootNode()->addChildNode(siameseNode);
//	auto scene = siameseBadTextureScene;

	
	
//	auto duckScene = TestSceneNamed("duck");
//	auto scene = duckScene;
	
//	auto palmScene = TestSceneNamed("cartoon_palm_tree");
////	auto palmNode = palmScene->rootNode()->childNode("palm_tree", true);
////	for (auto node : palmScene->rootNode()->allChildNodes()) {
////		cout << "node: " << node->name() << endl;
////	}
//	palmScene->rootNode()->scale(palmScene->rootNode()->scale() * 32.0f);
//	palmScene->rootNode()->position(vec3(0.0f, 47.0f, 0.0f));
//	scene->rootNode()->addChildNode(palmScene->rootNode());
//	for (auto n : palmScene->rootNode()->allChildNodes()) {
//		if (n->geometry()) {
//			for (auto m : n->geometry()->materials()) {
//				m->doubleSided(true);
//			}
//		}
//	}
	
//	auto importTestScene = TestSceneNamed("importTest");
//	auto scene = importTestScene;
	
	auto palletScene = TestSceneNamed("Pallet");
	palletScene->rootNode()->position(vec3(-35.0f, 35.0f, 0.0f));
	palletScene->rootNode()->scale(palletScene->rootNode()->scale() * 20.0f);
	palletScene->rootNode()->rotation(vec4(-1.0f, 0.0f, 0.0f, radians(-90.0f)));
	scene->rootNode()->addChildNode(palletScene->rootNode());
//	auto scene = palletScene;
	
//	auto mushroomScene = TestSceneNamed("mushroom");
//	auto scene = mushroomScene;
	
//	auto crocusScene = TestSceneNamed("FL43_Crocus_sp_Crocus_3ds/FL43_1");
//	auto scene = crocusScene;
	
//	auto tree1Scene = TestSceneNamed("tree1");
//	auto scene = tree1Scene;
	
	auto tunaScene = TestSceneNamed("tuna");
	tunaScene->rootNode()->position(vec3(-13.0f, -72.0f, 40.0f));
	tunaScene->rootNode()->scale(tunaScene->rootNode()->scale() * 1.8f);
	tunaScene->rootNode()->rotation(vec4(0.0f, 1.0f, 0.0f, radians(-90.0f)));
	scene->rootNode()->addChildNode(tunaScene->rootNode());
//	auto scene = tunaScene;

//	auto woodContainerScene = TestSceneNamed("WoodContainer");
//	auto scene = woodContainerScene;
	
//	auto crate1Scene = TestSceneNamed("Crate1");
//	auto scene = crate1Scene;

//	auto woddenCrateScene = TestSceneNamed("WoddenCrate);
//	auto scene = woddenCrateScene;
	
	
//	auto scene = make_shared<Scene>();
//	auto coneGeo = make_shared<Cone>(1.0, 1.0, 64, 64);
//	auto coneNode = make_shared<Node>(coneGeo);
//	auto red = make_shared<Color>(Color::Red());
//	auto coneMaterialProperty = make_shared<MaterialProperty>(red);
//	auto coneMaterial = make_shared<Material>();
//	coneMaterial->diffuse(coneMaterialProperty);
//	coneGeo->materials().push_back(coneMaterial);
//	scene->rootNode()->addChildNode(coneNode);
	
	
	// default material + specular color
//	auto boxNode = make_shared<Node>();
//	auto boxGeometry = make_shared<Box>(10, 10, 10);
//	boxNode->geometry(boxGeometry);
////	auto boxMaterial = make_shared<Material>();
////	boxMaterial->specularExponent(150);
////	//auto boxSpecularMaterialProperty = make_shared<MaterialProperty>(make_shared<Color>(Color::Red()));
////	auto specularMap = TestImageNamed("bw_checker_grid");
////	auto boxSpecularMaterialProperty = make_shared<MaterialProperty>(specularMap);
////	boxMaterial->specular(boxSpecularMaterialProperty);
////	boxGeometry->materials().push_back(boxMaterial);
//	scene->rootNode()->addChildNode(boxNode);
	
	
	
	
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
	
	
	

	auto background = make_shared<MaterialProperty>(TestCubeNamed("nebula1_blue", "png"));
//	auto background = make_shared<MaterialProperty>(TestCubeNamed("yokohama", "jpg"));
//	auto background = make_shared<MaterialProperty>(TestCubeNamed("fishermansBastion", "jpg"));
	scene->background(background);


	
//	auto backgroundColor = make_shared<Color>(151.0f/255.0f, 182.0f/255.0f, 214.0f/255.0f, 1.0f);
//	//auto backgroundColor = make_shared<Color>(Color::Lime());
//	auto background = make_shared<MaterialProperty>(backgroundColor);
//	scene->background(background);
	
	
	
	auto ambientLight = make_shared<Light>(LightType_Ambient, make_shared<Color>(Color::DarkGray()));
	auto ambientLightNode = make_shared<Node>(ambientLight);
	scene->rootNode()->addChildNode(ambientLightNode);
	
	
	
	const int NUM_RANDOM_LIGHTS = 50;
	auto colors = Color::Rainbow();
	for (int l=0; l<NUM_RANDOM_LIGHTS; ++l) {
		auto light = make_shared<Light>(LightType_Point);
		auto lightNode = make_shared<Node>(light);
		int randX = Random(-150, 150);
		int randY = Random(-150, 150);
		int randZ = Random(-150, 150);
		lightNode->position(vec3(randX, randY, randZ));
		auto color = make_shared<Color>(colors[Random(0, 18)]);
		light->color(color);
		cout << "Adding random light with position: "
		<< lightNode->position()<< ", color: " << *light->color() << endl;
		
		auto geometry = make_shared<Sphere>(5.0, 8);
		
		auto materialProperty = make_shared<MaterialProperty>(color);
		auto material = make_shared<Material>(materialProperty, materialProperty, nullptr);
		geometry->addMaterial(material);
		lightNode->geometry(geometry);
		
		scene->rootNode()->addChildNode(lightNode);
	}

	

	auto window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
	window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
	window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
	window.scene(scene);
	window.enableCursor(false);
	//window.maximumFramerate(240.0);

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
}

void Test::windowDidUpdateCallback(Scene& scene, float deltaSeconds) {

}
