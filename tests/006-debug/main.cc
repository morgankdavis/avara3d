//
//  main.cpp
//  avara3d
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include <memory>
#include <utility>

#include <glm/glm.hpp>

#include "a3d/a3d.h"
#include "a3d/Utilities.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;
using namespace std::placeholders;

const LogLevel				A3D_APP_LOG_LEVEL		{LogLevel::Debug};
const uvec2					WINDOW_SIZE				{1280, 768};
const bool					FULLSCREEN				{false};
const bool					ENABLE_HIGH_DPI			{true};
const AntialiasingMode		ANTIALIAS_MODE			{AntialiasingMode::None};
const bool					ENABLE_VSYNC			{false};
const bool					CAPTURE_CURSOR			{false};
const float					MOUSE_SENSITIVITY		{0.5};

void UpdateCallback(Scene& scene, double time, double deltaTime);
void WillRenderCallback(VisualWorld& world, double time, double deltaTime);
void DidRenderCallback(VisualWorld& world, double time, double deltaTime);

void InitLog();
void LogBuildInfo();

int main(int argc, const char* argv[]) {

	try {
		InitLog();
		LogBuildInfo();

		auto window = make_unique<GLFWWindow>(RenderingApi::OpenGL,
											  *utils::ExecutableName(),
											  WINDOW_SIZE,
											  FULLSCREEN,
											  ENABLE_HIGH_DPI,
											  ANTIALIAS_MODE);
		window->vSyncEnabled(ENABLE_VSYNC);
		window->cursorCaptured(CAPTURE_CURSOR);

		auto inputManager = make_unique<GLFWInputManager>(window.get());
		if (inputManager->errorMask() == DesktopInputManagerErrorMask::PermissionDenied) {
			A3D_APP_LOG_E("GLFWInputManager permission denied.");
			// on macOS 10.15 Catalina+, this is probably a permissions issue,
			// and the OS will alert the user.
			// just keep going and let the user decide what they want to do.
		}

		auto visualWorld = make_unique<VisualWorld>(*window);
		visualWorld->fogStartDistance(500.0);
		visualWorld->fogEndDistance(5000.0);
		visualWorld->fogDensityExponent(1.0);
		visualWorld->fogColor(Color::LightGray());
		visualWorld->background(make_shared<Texture>(utils::CubeImageNamed("sky1", "png")));
		visualWorld->willRenderCallback(bind(&WillRenderCallback, _1, _2, _3));
		visualWorld->didRenderCallback(bind(&DidRenderCallback, _1, _2, _3));

		auto scene = make_unique<Scene>(std::move(visualWorld), nullptr, std::move(inputManager));
//	DebugOptions debugOptions = DebugOptions::None;
//	debugOptions = A3D_MASK_ADD(debugOptions, DebugOptions::ShowStatsOverlay);
//	debugOptions = A3D_MASK_ADD(debugOptions, DebugOptions::ShowBoundingBoxes);
		DebugOptions debugOptions = DebugOptions::ShowStatsOverlay
									| DebugOptions::ShowBoundingBoxes;
		scene->debugOptions(debugOptions);
		scene->updateCallback(bind(&UpdateCallback, _1, _2, _3));

		//auto ambientLight = make_shared<Light>(LightType::Ambient, make_shared<Color>(0.25f, 0.25, 0.25, 1.0));
		auto ambientLight = make_shared<AmbientLight>(Color::Gray());
		auto ambientLightNode = make_shared<Node>("Ambient light");
		ambientLightNode->light(ambientLight);
		scene->rootNode()->addChild(ambientLightNode);

		//auto pointLight = make_shared<Light>(LightType::Point, Color::White());
		auto pointLight = make_shared<PointLight>(Color::White());
		pointLight->quadraticAttenuation(0.0001);
		auto pointLightNode = make_shared<Node>();
		pointLightNode->light(pointLight);
		scene->rootNode()->addChild(pointLightNode);
		pointLightNode->position({100.0, 20.0, 20.0});

		auto materialProperty = pointLight->color();
		auto material = make_shared<Material>();
		material->name("LIGHT material");
		material->emission(materialProperty);
		auto mesh = shared_ptr(Sphere::Mesh(3.5, 4, material));
//	mesh->addMaterial(material);
//	mesh->replaceMaterial(0, material); // TODO: EHHHHHHHH??????????/
		pointLightNode->mesh(mesh);

		auto teapotNode = Node::MeshNode(utils::MeshNamed("teapot/teapot"));
		teapotNode->rotation({1, 0, 0}, radians(30.0));
		teapotNode->scale(teapotNode->scale() * 50.0f);
		scene->rootNode()->addChild(teapotNode);

		auto dragonNode = Node::MeshNode(utils::MeshNamed("dragon/dragon"));
		dragonNode->scale({2.5, 2.5, 2.5});
		dragonNode->position({50, 0, 0});

		scene->rootNode()->addChild(dragonNode);

		auto boxNode = Node::MeshNode(Box::Mesh(1.0, 1.0, 1.0));
		scene->rootNode()->addChild(boxNode);

		window->center();
		window->open();

		do {
			scene->update();
		} while (window->isOpen());
	}
	catch (Exception& e) {
		A3D_APP_LOG_F("Exception: {}", e.what());
		return -1;
	}

	return 0;
}

/// Scene Callbacks ///

void UpdateCallback(Scene& scene, double time, double deltaTime) {

	auto window = dynamic_cast<GLFWWindow*>(scene.visualWorld()->renderContext());

	// get input

	auto im = static_cast<DesktopInputManager*>(scene.inputManager());

	auto keysPressed = im->keysPressed();

	if (keysPressed.count(Key::Escape)) {
		window->close();
	}

	if (keysPressed.count(Key::Slash)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(Key::F)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowWireframes)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(),
											  DebugOptions::ShowWireframes));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(),
										   DebugOptions::ShowWireframes));
		}
	}
	if (keysPressed.count(Key::B)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowBoundingBoxes)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(),
											  DebugOptions::ShowBoundingBoxes));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(),
										   DebugOptions::ShowBoundingBoxes));
		}
	}
	if (keysPressed.count(Key::I)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowStatsOverlay)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(),
											  DebugOptions::ShowStatsOverlay));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(),
										   DebugOptions::ShowStatsOverlay));
		}
	}

	if (keysPressed.count(Key::V)) {
		window->vSyncEnabled(!(window->vSyncEnabled()));
	}

	if (keysPressed.count(Key::Backslash)) {
		utils::SaveSnapshot(*window);
	}

	if (keysPressed.count(Key::R)) {
		if (!window->recordingGIF()) {
			utils::StartGIFRecording(*window, {320, 240}, 8);
		}
		else {
			utils::StopGIFRecording(*window);
		}
	}

	if (window->cursorCaptured()) {

		// mouselook

		vec2 mousePositionDelta = im->mousePositionDelta();

		auto pov = scene.visualWorld()->pointOfView().lock();
		if (pov) {

			// look

			vec3 camForward = pov->worldForward();
			vec3 camRight = pov->worldRight();
			vec3 camUp = pov->worldUp();

			static const float MOUSE_SPEED_SCALAR = .002;
			static const float MOUSE_SPEED = MOUSE_SENSITIVITY * MOUSE_SPEED_SCALAR;

			float deltaRotX = math::atan(MOUSE_SPEED * mousePositionDelta.x);
			float deltaRotY = math::atan(MOUSE_SPEED * mousePositionDelta.y);

			vec3 angles = pov->eulerAngles();
			pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));

			// move

			static float MOVE_SPEED = utils::Max(scene.rootNode()->extent());

			auto keysDown = im->keysDown();

			if (keysDown.count(Key::W)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * camForward;
				pov->position(pov->position() + positionDelta);
			} else if (keysDown.count(Key::S)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * -camForward;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(Key::A)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * -camRight;
				pov->position(pov->position() + positionDelta);
			} else if (keysDown.count(Key::D)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * camRight;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(Key::Space)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * camUp;
				pov->position(pov->position() + positionDelta);
			}
		}
	}
}

/// VisualWorld Callbacks ///

void WillRenderCallback(VisualWorld& world, double time, double deltaTime) {

}

void DidRenderCallback(VisualWorld& world, double time, double deltaTime) {

}

/// Static ///

void InitLog() {

	string executableName = *utils::ExecutableName();
	auto nativeSink = make_unique<StdOutLogSink>();
	auto fileSink = make_unique<FileLogSink>(*(utils::ExecutableDirectory())
											 / (executableName + string(".log")));
	auto sinks = unordered_set<unique_ptr<LogSink>>();
	sinks.insert(std::move(nativeSink));
	sinks.insert(std::move(fileSink));

	auto appLog = make_unique<Log>(executableName, std::move(sinks));
	appLog->level(A3D_APP_LOG_LEVEL);
	Log::AppLog(std::move(appLog));

	Log::MainLog().level(A3D_APP_LOG_LEVEL);

//	for (unsigned l=0; l < 50000; ++l) {
//		A3D_A3D_APP_LOG_I("line {}", l);
//		A3D_APP_LOG_I(logger, "line {}", l);
//	}
}

void LogBuildInfo() {

	auto buildInfo = BuildInfo::Info();
	auto version = buildInfo.version();
	A3D_APP_LOG_I("A3D version: {}.{}.{}", version.major, version.minor, version.patch);
	A3D_APP_LOG_I("Build: {}", buildInfo.number());
	A3D_APP_LOG_I("Type: {}", buildInfo.type() == BuildInfo::Type::Debug ? "Debug" : "Release");
	A3D_APP_LOG_I("Origin: {}", buildInfo.origin() == BuildInfo::Origin::CI ? "CI" : "AdHoc");
}

