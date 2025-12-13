//
//  main.cpp
//  avara3d
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include <iostream>
#include <memory>
#include <utility>

#include "a3d/a3d.h"
#include "a3d/Utilities.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;
using namespace std::placeholders;

const uvec2					WINDOW_SIZE			{1280, 768};
const bool					FULLSCREEN			{false};
const bool					ENABLE_HIGH_DPI		{true};
const AntialiasingMode		ANTIALIAS_MODE		{AntialiasingMode::Msaa4X};
const bool					ENABLE_VSYNC		{false};
const bool					CAPTURE_CURSOR		{false};

void UpdateCallback(Scene& scene, double time, double deltaTime);
void WillRenderCallback(VisualWorld& world, double time, double deltaTime);
void DidRenderCallback(VisualWorld& world, double time, double deltaTime);

a3d::Node*	g_importMeshRoot;

int main(int argc, const char* argv[]) {

	cout << "test001::main()\n" << endl;

	auto window = make_unique<GLFWWindow>(RenderingApi::OpenGL,
										  *utils::ExecutableName(),
										  WINDOW_SIZE,
										  FULLSCREEN,
										  ENABLE_HIGH_DPI,
										  ANTIALIAS_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_unique<VisualWorld>(*window);
//	visualWorld->autoEnablesDefaultLighting(false);
	auto backgroundColor = make_shared<Color>(u8vec3{109, 136, 164});
	auto background = MaterialProperty(backgroundColor);
	visualWorld->background(background); // TODO: is this copying?
	visualWorld->willRenderCallback(bind(&WillRenderCallback, _1, _2, _3));
	visualWorld->didRenderCallback(bind(&DidRenderCallback, _1, _2, _3));

	auto scene = make_unique<Scene>();
	scene->visualWorld(std::move(visualWorld));
	scene->updateCallback(bind(&UpdateCallback, _1, _2, _3));

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

	auto testScene = utils::SceneNamed("import_test/import_test", options);

	auto testSceneNodes = testScene->rootNode()->children();
	auto importLightsCamerasRoot = make_shared<Node>("importLightsCamerasRoot");
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

	window->center();
	window->open();

	do {
		scene->update();
	} while (window->isOpen());

	return 0;
}

/// Scene Callbacks ///

void UpdateCallback(Scene& scene, double time, double deltaTime) {

	float rotationDeg = deltaTime * 30.0; // 30deg/sec

	g_importMeshRoot->transform(rotate(g_importMeshRoot->transform(),
									   radians(rotationDeg),
									   {0.0f, 1.0f, 0.0f}));

	static float timeAccum = 0;
	static unsigned frames = 0;
	timeAccum += deltaTime;
	++frames;
	if (timeAccum >= 1.0) {
		cout << ((float)frames)/timeAccum << " fps" << endl;
		timeAccum = 0;
		frames = 0;
	}
}

/// VisualWorld Callbacks ///

void WillRenderCallback(VisualWorld& world, double time, double deltaTime) {

}

void DidRenderCallback(VisualWorld& world, double time, double deltaTime) {

}
