//
//  main.cpp
//  006-debug
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include <memory>
#include "a3d/Application.h"
#include "App.h"

int main(int argc, char* argv[]) {
	return a3d::Application::Run(std::make_unique<test::debug::App>(argc, argv));
}


//
//  main.cpp
//  avara3d
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include <memory>
#include <utility>
#include <vector>

#include "a3d/a3d.h"
#include "a3d/util/filesystem.h"
#include "a3d/util/snapshot.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;
using namespace std::placeholders;

const Log::Level						APP_LOG_LEVEL		{Log::Level::Debug};
const uvec2								WINDOW_SIZE			{1280, 768};
const bool								FULLSCREEN			{false};
const bool								ENABLE_HIGH_DPI		{true};
const RenderContext::AntialiasingMode	ANTIALIAS_MODE		{RenderContext::AntialiasingMode::None};
const bool								ENABLE_VSYNC		{false};
const bool								CAPTURE_CURSOR		{false};
const float								MOUSE_SENSITIVITY	{0.5};

void UpdateCallback(Scene& scene, double time, double deltaTime);
void WillRenderCallback(VisualWorld& world, double time, double deltaTime);
void DidRenderCallback(VisualWorld& world, double time, double deltaTime);

void InitLog();
void LogBuildInfo();

int main(int argc, const char* argv[]) {

	try {
		InitLog();
		LogBuildInfo();

		auto window = make_unique<GLFWWindow>(RenderContext::RenderingApi::OpenGL,
											  *util::filesystem::ExecutableName(),
											  WINDOW_SIZE,
											  FULLSCREEN,
											  ENABLE_HIGH_DPI,
											  ANTIALIAS_MODE);
		window->vSyncEnabled(ENABLE_VSYNC);
		window->cursorCaptured(CAPTURE_CURSOR);

		auto inputManager = make_unique<GLFWInputManager>(window.get());

		auto visualWorld = make_unique<VisualWorld>(*window);
		visualWorld->fogStartDistance(500.0);
		visualWorld->fogEndDistance(5000.0);
		visualWorld->fogDensityExponent(1.0);
		visualWorld->fogColor(Color::LightGray());
		visualWorld->background(make_shared<Texture>(util::filesystem::CubeImageNamed("sky1", "png")));
		visualWorld->willRenderCallback(bind(&WillRenderCallback, _1, _2, _3));
		visualWorld->didRenderCallback(bind(&DidRenderCallback, _1, _2, _3));

		auto scene = make_unique<Scene>(std::move(visualWorld), nullptr, std::move(inputManager));
//	DebugOptions debugOptions = DebugOptions::None;
//	debugOptions = util::bitmask::add(debugOptions, DebugOptions::ShowStatsOverlay);
//	debugOptions = util::bitmask::add(debugOptions, DebugOptions::ShowBoundingBoxes);
		auto debugOptions = Scene::DebugOptions::ShowStatsOverlay
							| Scene::DebugOptions::ShowBoundingBoxes;
		scene->debugOptions(debugOptions);
		scene->updateCallback(bind(&UpdateCallback, _1, _2, _3));

		//auto ambientLight = make_shared<Light>(LightType::Ambient, make_shared<Color>(0.25f, 0.25, 0.25, 1.0));
		auto ambientLight = make_shared<AmbientLight>(Color::Gray());
		auto ambientLightNode = make_shared<Node>("Ambient light");
		ambientLightNode->light(ambientLight);
		scene->rootNode()->addChild(ambientLightNode);

		//auto pointLight = make_shared<Light>(LightType::Point, Color::White());
		auto pointLight = make_shared<PointLight>(Color::White());
		pointLight->attenuation(Attenuation{.quadratic = 0.0001f});
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

		auto teapotNode = Node::MeshNode(util::filesystem::MeshNamed("teapot/teapot"));
		teapotNode->rotation({1, 0, 0}, radians(30.0));
		teapotNode->scale(teapotNode->scale() * 50.0f);
		scene->rootNode()->addChild(teapotNode);

		auto dragonNode = Node::MeshNode(util::filesystem::MeshNamed("dragon/dragon"));
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
	catch (std::exception& e) {
		log::app::f()("Exception: {}", e.what());
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

	using Key = DesktopInputManager::Key;
	using MouseButton = DesktopInputManager::MouseButton;

	if (keysPressed.count(Key::Escape)) {
		window->close();
	}

	if (keysPressed.count(Key::Slash)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	using DebugOptions = Scene::DebugOptions;

	if (keysPressed.count(Key::F)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowWireframes)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
											  DebugOptions::ShowWireframes));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(),
										   DebugOptions::ShowWireframes));
		}
	}
	if (keysPressed.count(Key::B)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowBoundingBoxes)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
											  DebugOptions::ShowBoundingBoxes));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(),
										   DebugOptions::ShowBoundingBoxes));
		}
	}
	if (keysPressed.count(Key::I)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowStatsOverlay)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
											  DebugOptions::ShowStatsOverlay));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(),
										   DebugOptions::ShowStatsOverlay));
		}
	}

	if (keysPressed.count(Key::V)) {
		window->vSyncEnabled(!(window->vSyncEnabled()));
	}

	if (keysPressed.count(Key::Backslash)) {
		util::snapshot::SaveSnapshot(*window);
	}

	if (keysPressed.count(Key::R)) {
		if (!window->recordingGIF()) {
			util::snapshot::StartGIFRecording(*window, {320, 240}, 8);
		}
		else {
			util::snapshot::StopGIFRecording(*window);
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

			static float MOVE_SPEED = math::max(scene.rootNode()->extent());

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

	string executableName = *util::filesystem::ExecutableName();

	auto nativeSink = make_unique<StdOutLogSink>();
	auto fileSink = make_unique<FileLogSink>(*(util::filesystem::ExecutableDirectory())
											 / (executableName + string(".log")));
	auto sinks = vector<unique_ptr<LogSink>>();
	sinks.push_back(std::move(nativeSink));
	sinks.push_back(std::move(fileSink));

	Log appLog{executableName, std::move(sinks)};
	appLog.level(APP_LOG_LEVEL);
	Log::AppLog(std::move(appLog));
}

void LogBuildInfo() {

	auto buildInfo = BuildInfo::Info();
	log::app::i()("A3D version: {}", BuildInfo::VersionString(buildInfo.version()));
	log::app::i()("Build: {}", buildInfo.number());
	log::app::i()("Type: {}", BuildInfo::TypeString(buildInfo.type()));
	log::app::i()("Origin: {}", BuildInfo::OriginString(buildInfo.origin()));
}

