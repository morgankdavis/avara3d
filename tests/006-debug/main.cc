//
//  main.cpp
//	avara3d
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include <memory>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

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


void InitLog();


constexpr LogLevel				LOG_LEVEL =				LogLevel::Debug;
constexpr bool					ENABLE_HIGH_DPI =		true;
constexpr unsigned				WINDOW_WIDTH =			800;
constexpr unsigned				WINDOW_HEIGHT =			600;
constexpr bool					FULLSCREEN =			false;
constexpr AntialiasingMode		ANTIALIAS_MODE =		AntialiasingMode::None;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr float					MOUSE_SENSITIVITY =		0.5;


std::unique_ptr<a3d::Logger>	logger;


int main(int argc, const char* argv[]) {

	InitLog();

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
	visualWorld->fogStartDistance(500.0);
	visualWorld->fogEndDistance(5000.0);
	visualWorld->fogDensityExponent(1.0);
	visualWorld->fogColor(Color::LightGray());
	visualWorld->background(make_shared<Texture>(CubeImageNamed("sky1", "png")));
	visualWorld->willRender(bind(&WillRenderCallback, _1, _2));
	visualWorld->didRender(bind(&DidRenderCallback, _1, _2));

	auto inputManager = make_unique<WindowInputManager>(window.get());

	auto scene = make_unique<Scene>(std::move(visualWorld), nullptr, std::move(inputManager));
//	DebugOptions debugOptions = DebugOptions::None;
//	debugOptions = A3D_MASK_ADD(debugOptions, DebugOptions::ShowStatsOverlay);
//	debugOptions = A3D_MASK_ADD(debugOptions, DebugOptions::ShowBoundingBoxes);
	DebugOptions debugOptions = DebugOptions::ShowStatsOverlay
								| DebugOptions::ShowBoundingBoxes;
	scene->debugOptions(debugOptions);
	scene->update(bind(&UpdateCallback, _1, _2));

	auto ambientLight = make_shared<Light>(LightType::Ambient, make_shared<Color>(0.25f, 0.25, 0.25, 1.0));
	auto ambientLightNode = make_shared<Node>("Ambient light");
	ambientLightNode->light(ambientLight);
	scene->rootNode()->addChild(ambientLightNode);

	auto pointLight = make_shared<Light>(LightType::Point, Color::White());
	pointLight->attenuationFactor(0.0000015);
	auto pointLightNode = make_shared<Node>();
	pointLightNode->light(pointLight);
	scene->rootNode()->addChild(pointLightNode);
	pointLightNode->position({100.0, 20.0, 20.0});

	auto materialProperty = pointLight->color();
	auto material = make_shared<Material>();
	material->name("LIGHT material");
	material->emission(materialProperty);
	auto mesh = shared_ptr(std::move(Sphere::Mesh(3.5, 4, material)));
//	mesh->addMaterial(material);
//	mesh->replaceMaterial(0, material); // TODO: EHHHHHHHH??????????/
	pointLightNode->mesh(mesh);

	auto teapotNode = Node::MeshNode(MeshNamed("teapot/teapot"));
	teapotNode->rotation({1, 0, 0}, radians(30.0));
	teapotNode->scale(teapotNode->scale() * 50.0f);
	scene->rootNode()->addChild(teapotNode);

	auto dragonNode = Node::MeshNode(MeshNamed("dragon/dragon"));
	dragonNode->scale({2.5, 2.5, 2.5});
	dragonNode->position({50, 0, 0});

	scene->rootNode()->addChild(dragonNode);

	auto boxNode = Node::MeshNode(Box::Mesh(1.0, 1.0, 1.0));
	scene->rootNode()->addChild(boxNode);

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

	auto window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());

	// get input

	auto keysPressed = scene.inputManager()->keysPressed();

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
		SaveSnapshot(*window);
	}

	if (keysPressed.count(Key::R)) {
		if (!window->recordingGIF()) {
			StartGIFRecording(*window, 320, 8);
		}
		else {
			StopGIFRecording(*window);
		}
	}

	if (window->cursorCaptured()) {

		// mouselook

		vec2 mousePositionDelta = scene.inputManager()->mousePositionDelta();

		auto pov = scene.visualWorld()->pointOfView().lock();
		if (pov) {

			// look

			vec3 camForward = pov->worldForward();
			vec3 camRight = pov->worldRight();
			vec3 camUp = pov->worldUp();

			static const float MOUSE_SPEED_SCALAR = .002;
			static const float MOUSE_SPEED = MOUSE_SENSITIVITY * MOUSE_SPEED_SCALAR;

			float deltaRotX = atan(MOUSE_SPEED * mousePositionDelta.x);
			float deltaRotY = atan(MOUSE_SPEED * mousePositionDelta.y);

			vec3 angles = pov->eulerAngles();
			pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));

			// move

			static float MOVE_SPEED = Max(scene.rootNode()->extent());

			auto keysDown = scene.inputManager()->keysDown();

			if (keysDown.count(Key::W)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
				pov->position(pov->position() + positionDelta);
			} else if (keysDown.count(Key::S)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(Key::A)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
				pov->position(pov->position() + positionDelta);
			} else if (keysDown.count(Key::D)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(Key::Space)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
				pov->position(pov->position() + positionDelta);
			}
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

/***************************************************************************************
	Static
 ***************************************************************************************/

void InitLog() {

	string executableName = *utils::ExecutableName();
	auto nativeSink = make_unique<StdOutLoggerSink>();
	auto fileSink = make_unique<FileLoggerSink>(*(utils::ExecutableDirectory())
												/ (executableName + string(".log")));
	auto sinks = unordered_set<unique_ptr<LoggerSink>>();
	sinks.insert(std::move(nativeSink));
	sinks.insert(std::move(fileSink));

	logger = make_unique<Logger>(executableName, std::move(sinks));
	logger->level(LOG_LEVEL);

	Logger::MainLogger().level(LOG_LEVEL);

//	for (unsigned l=0; l < 50000; ++l) {
//		A3D_LOG_I("line {}", l);
//		LOG_I(logger, "line {}", l);
//	}
}
