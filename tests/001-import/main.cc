//
//  main.cpp
//	avara3d
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include <iostream>
#include <memory>
#include <utility>

#include "glm/gtc/matrix_transform.hpp"

#include "a3d/a3d.h"
#include "a3d/Utilities.h"


using namespace a3d;
using namespace a3d::utils;
using namespace glm;
using namespace std;
using namespace std::placeholders;


void UpdateCallback(Scene& scene, float time);
void WillRenderCallback(VisualWorld& world, float time);
void DidRenderCallback(VisualWorld& world, float time);


constexpr bool					USE_HIGH_DPI =			false;
constexpr unsigned				WINDOW_WIDTH =			1024;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr AntialiasingMode		ANTIALIAS_MODE =		AntialiasingMode::Msaa4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;


a3d::Node* 		g_importLightsCamerasRoot;
a3d::Node* 		g_importMeshRoot;


int main(int argc, const char* argv[]) {

	cout << "test001::main()\n" << endl;

	auto window = make_unique<Window>(RenderingApi::OpenGL,
									  *utils::ExecutableName(),
									  WINDOW_WIDTH,
									  WINDOW_HEIGHT,
									  FULLSCREEN,
									  USE_HIGH_DPI,
									  ANTIALIAS_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_unique<VisualWorld>(window.get());
	auto backgroundColor = make_shared<Color>(109.0f / 255.0f, 136.0f / 255.0f, 164.0f / 255.0f, 1.0f);
	auto background = MaterialProperty(backgroundColor);
	visualWorld->background(background); // TODO: is this copying?
	visualWorld->willRender(bind(&WillRenderCallback, _1, _2));
	visualWorld->didRender(bind(&DidRenderCallback, _1, _2));

	auto scene = make_unique<Scene>();
	scene->visualWorld(std::move(visualWorld));
	scene->update(bind(&UpdateCallback, _1, _2));

//	auto options = SceneImportOptions::ImportAll;
//	auto options = SceneImportOptions::ImportMeshes;
	auto options = SceneImportOptions::ImportMeshes
				   | SceneImportOptions::ImportMaterials;
//	auto options = SceneImportOptions::ImportMeshes
//				   | SceneImportOptions::ImportMaterials
//				   | SceneImportOptions::ImportLights;
//	auto options = SceneImportOptions::ImportMeshes
//				   | SceneImportOptions::ImportMaterials
//				   | SceneImportOptions::ImportLights
//				   | SceneImportOptions::ImportCameras;
//	auto options = SceneImportOptions::ImportLights
//				   | SceneImportOptions::ImportCameras;

	auto testScene = SceneNamed("import_test/import_test", options);

	auto testSceneNodes = testScene->rootNode()->children();
	auto importLightsCamerasRoot = make_shared<Node>("importLightsCamerasRoot");
	g_importLightsCamerasRoot = importLightsCamerasRoot.get();
	auto importMeshRoot = make_shared<Node>("importMeshRoot");
	g_importMeshRoot = importMeshRoot.get();

	for (auto& node : testSceneNodes) {

		if (node->light() || node->camera()) {
			importLightsCamerasRoot->addChild(node);
		}
		else {
			importMeshRoot->addChild(node);
		}
	}


//	importMeshRoot = testScene->rootNode();

	scene->rootNode()->addChild(importLightsCamerasRoot);
	scene->rootNode()->addChild(importMeshRoot);

	window->open();
	scene->run();

	return 0;
}

/***************************************************************************************
	Scene Callbacks
 ***************************************************************************************/

void UpdateCallback(Scene& scene, float time) {

	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;

	float rotationDeg = deltaSeconds * 30.0; // 30deg/sec

	g_importMeshRoot->transform(rotate(g_importMeshRoot->transform(),
									   radians(rotationDeg),
									   {0.0f, 1.0f, 0.0f}));

	static float timeAccum = 0;
	static unsigned frames = 0;
	timeAccum += deltaSeconds;
	++frames;
	if (timeAccum >= 1.0) {
		cout << ((float)frames)/timeAccum << " fps" << endl;
		timeAccum = 0;
		frames = 0;
	}
}

/***************************************************************************************
	VisualWorld Callbacks
 ***************************************************************************************/

void WillRenderCallback(VisualWorld& world, float time) {

}

void DidRenderCallback(VisualWorld& world, float time) {

}
