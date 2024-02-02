//
//  main.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include <iostream>
#include <memory>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ae/ae.h"
#include "ae/Utilities.h"


using namespace ae;
using namespace ae::utils;
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
constexpr ANTIALIASING_MODE		ANTIALIAS_MODE =		ANTIALIASING_MODE::MSAA_4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr float					MOUSE_SENSITIVITY =		0.5;


int main(int argc, const char* argv[]) {

	cout << "test003::main()\n" << endl;

	auto window = make_shared<Window>(RENDER_API::OPENGL,
									  FULLSCREEN,
									  WINDOW_WIDTH,
									  WINDOW_HEIGHT,
									  USE_HIGH_DPI,
									  ANTIALIAS_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_shared<VisualWorld>(window);
	auto backgroundColor = make_shared<Color>(109.0f/255.0f, 136.0f/255.0f, 164.0f/255.0f, 1.0f);
	visualWorld->background(make_shared<MaterialProperty>(backgroundColor));
	visualWorld->willRender(bind(&WillRenderCallback, _1, _2));
	visualWorld->didRender(bind(&DidRenderCallback, _1, _2));

	auto inputManager = make_shared<WindowInputManager>(window);

	auto scene = SceneNamed("import_test/import_test");
	scene->visualWorld(visualWorld);
	scene->inputManager(inputManager);
	scene->update(bind(&UpdateCallback, _1, _2));

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

	auto window = static_pointer_cast<Window>(scene.visualWorld()->renderContext());

	// get input

	auto keysDown = scene.inputManager()->keysDown();
	for (auto k : keysDown) {
		cout << "Key: " << static_cast<underlying_type<KEY>::type>(k) << endl;
	}

	auto keysPressed = scene.inputManager()->keysPressed();
	if (keysPressed.count(KEY::SLASH)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(KEY::ESCAPE)) {
		exit(0);
	}

	for (auto mb : scene.inputManager()->mouseButtonsDown()) {
		cout << "Mouse button: " << static_cast<underlying_type<MOUSE_BUTTON>::type>(mb) << endl;
	}

	vec2 mousePositionDelta = scene.inputManager()->mousePositionDelta();
	//	if (mousePositionDelta.x || mousePositionDelta.y) {
	//		cout << "Mouse move delta: (" << mousePositionDelta.x << ", " << mousePositionDelta.y << ")" << endl;
	//	}

	vec2 mouseScrollWheelDelta = scene.inputManager()->mouseScrollWheelDelta();
	if (mouseScrollWheelDelta.x || mouseScrollWheelDelta.y) {
		cout << "Mouse scroll wheel delta: (" << mouseScrollWheelDelta.x << ", "
			 << mouseScrollWheelDelta.y << ")" << endl;
	}

	// move camera

	auto pov = scene.visualWorld()->pointOfView();
	if (pov) {

		// look

		vec3 camForward = pov->worldForward();
		vec3 camRight = pov->worldRight();
		vec3 camUp = pov->worldUp();

		// tanA = y/x
		// tanA = mouseDelta / distance
		// A = atan(mouseDelta / distance)

		static const float MOUSE_SPEED_SCALAR = .002;
		static const float MOUSE_SPEED = MOUSE_SENSITIVITY * MOUSE_SPEED_SCALAR;

		float deltaRotX = atan(MOUSE_SPEED * mousePositionDelta.x);
		float deltaRotY = atan(MOUSE_SPEED * mousePositionDelta.y);

		vec3 angles = pov->eulerAngles();
		// weird angles
		//_cameraNode->eulerAngles(vec3(angles.x + -deltaRotX, 0, angles.z + deltaRotY));
		// pitch, yaw, roll
		pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));


		// move

		static float MOVE_SPEED = Max(scene.rootNode()->extent());

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

		if(keysDown.count(KEY::SPACE)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
			pov->position(pov->position() + positionDelta);
		}
	}
}

/***************************************************************************************
	VisualWorld Callbacks
 ***************************************************************************************/

void WillRenderCallback(VisualWorld& world, float time) {
}

void DidRenderCallback(VisualWorld& world, float time) {

}
