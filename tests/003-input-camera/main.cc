//
//  main.cpp
//  avara3d
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "a3d/a3d.h"
#include "a3d/Utilities.h"


using namespace a3d;
using namespace glm;
using namespace std;
using namespace std::placeholders;


constexpr bool					ENABLE_HIGH_DPI =		true;
constexpr unsigned				WINDOW_WIDTH =			1024;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr AntialiasingMode		ANTIALIAS_MODE =		AntialiasingMode::Msaa4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr float					MOUSE_SENSITIVITY =		0.5;


void UpdateCallback(Scene& scene, float time, float deltaTime);
void WillRenderCallback(VisualWorld& world, float time, float deltaTime);
void DidRenderCallback(VisualWorld& world, float time, float deltaTime);


int main(int argc, const char* argv[]) {

	cout << "test003::main()\n" << endl;

	auto window = make_unique<Window>(RenderingApi::OpenGL,
									  *utils::ExecutableName(),
									  WINDOW_WIDTH,
									  WINDOW_HEIGHT,
									  FULLSCREEN,
									  ENABLE_HIGH_DPI,
									  ANTIALIAS_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_unique<VisualWorld>(window.get());
	auto backgroundColor = make_shared<Color>(109.0f/255.0f, 136.0f/255.0f, 164.0f/255.0f, 1.0f);
	visualWorld->background(backgroundColor);
	visualWorld->willRender(bind(&WillRenderCallback, _1, _2, _3));
	visualWorld->didRender(bind(&DidRenderCallback, _1, _2, _3));

	auto inputManager = make_unique<WindowInputManager>(window.get());

	auto scene = utils::SceneNamed("import_test/import_test");
	scene->visualWorld(std::move(visualWorld));
	scene->inputManager(std::move(inputManager));
	scene->update(bind(&UpdateCallback, _1, _2, _3));

	window->open();
	scene->run();

	return 0;
}

/***************************************************************************************
	Scene Callbacks
 ***************************************************************************************/

void UpdateCallback(Scene& scene, float time, float deltaTime) {

	auto window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());

	// get input

	auto keysDown = scene.inputManager()->keysDown();
	for (auto k : keysDown) {
		cout << "Key: " << static_cast<underlying_type<Key>::type>(k) << endl;
	}

	auto keysPressed = scene.inputManager()->keysPressed();
	if (keysPressed.count(Key::Slash)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(Key::Escape)) {
		exit(0);
	}

	for (auto mb : scene.inputManager()->mouseButtonsDown()) {
		cout << "Mouse button: " << static_cast<underlying_type<MouseButton>::type>(mb) << endl;
	}

	vec2 mouseScrollWheelDelta = scene.inputManager()->mouseScrollWheelDelta();
	if (mouseScrollWheelDelta.x > 0 || mouseScrollWheelDelta.y > 0) {
		cout << "Mouse scroll wheel delta: (" << mouseScrollWheelDelta.x << ", "
			 << mouseScrollWheelDelta.y << ")" << endl;
	}

	// move camera

	if (auto pov = scene.visualWorld()->pointOfView().lock()) {

		// look

		vec3 camForward = pov->worldForward();
		vec3 camRight = pov->worldRight();
		vec3 camUp = pov->worldUp();

		// tanA = y/x
		// tanA = mouseDelta / distance
		// A = atan(mouseDelta / distance)

		static const float MOUSE_SPEED_SCALAR = .002;
		static const float MOUSE_SPEED = MOUSE_SENSITIVITY * MOUSE_SPEED_SCALAR;

		vec2 mousePositionDelta = scene.inputManager()->mousePositionDelta();
		float deltaRotX = atan(MOUSE_SPEED * mousePositionDelta.x);
		float deltaRotY = atan(MOUSE_SPEED * mousePositionDelta.y);

		vec3 angles = pov->eulerAngles();
		// weird angles
		//_cameraNode->eulerAngles(vec3(angles.x + -deltaRotX, 0, angles.z + deltaRotY));
		// pitch, yaw, roll
		pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));


		// move

		static float MOVE_SPEED = utils::Max(scene.rootNode()->extent());

		if(keysDown.count(Key::W)) {
			vec3 positionDelta = deltaTime * MOVE_SPEED * camForward;
			pov->position(pov->position() + positionDelta);
		}
		else if(keysDown.count(Key::S)) {
			vec3 positionDelta = deltaTime * MOVE_SPEED * -camForward;
			pov->position(pov->position() + positionDelta);
		}

		if(keysDown.count(Key::A)) {
			vec3 positionDelta = deltaTime * MOVE_SPEED * -camRight;
			pov->position(pov->position() + positionDelta);
		}
		else if(keysDown.count(Key::D)) {
			vec3 positionDelta = deltaTime * MOVE_SPEED * camRight;
			pov->position(pov->position() + positionDelta);
		}

		if(keysDown.count(Key::Space)) {
			vec3 positionDelta = deltaTime * MOVE_SPEED * camUp;
			pov->position(pov->position() + positionDelta);
		}
	}
}

/***************************************************************************************
	VisualWorld Callbacks
 ***************************************************************************************/

void WillRenderCallback(VisualWorld& world, float time, float deltaTime) {
}

void DidRenderCallback(VisualWorld& world, float time, float deltaTime) {

}
