//
//  main.cpp
//	avara-engine
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include <iostream>
#include <memory>
#include <vector>

#include "glm/glm.hpp"

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


void SetAllFilterModes(FILTER_MODE mode, Scene& scene);
void SetAllMaxAnisotropy(float anisotropy, Scene& scene);
void ProcessEdit(Node& node, set<KEY>& keysDown, set<KEY>& keysPressed);


constexpr bool					USE_HIGH_DPI =			false;
constexpr unsigned				WINDOW_WIDTH =			1024;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr ANTIALIASING_MODE		ANTIALIAS_MODE =		ANTIALIASING_MODE::MSAA_4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr bool 					ORTHO_CAMERA =			false;
constexpr float					MOUSE_SENSITIVITY =		0.5;


std::shared_ptr<ae::Logger>		logger;
std::shared_ptr<ae::Node>		pointLightNode;


int main(int argc, const char* argv[]) {

	logger = make_shared<Logger>("test-005", Logger::MainLogger()->sinks());
	logger->level(LogLevel::Debug);
	Logger::MainLogger()->level(LogLevel::Debug);
	LOG_I(logger, "");

	auto window = make_shared<Window>(RENDER_API::OPENGL,
									  FULLSCREEN,
									  WINDOW_WIDTH,
									  WINDOW_HEIGHT,
									  USE_HIGH_DPI,
									  ANTIALIAS_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_shared<VisualWorld>(window);
	visualWorld->fogStartDistance(500.0);
	visualWorld->fogEndDistance(5000.0);
	visualWorld->fogDensityExponent(1.0);
	visualWorld->fogColor(Color::LightGray());
	visualWorld->willRender(bind(&WillRenderCallback, _1, _2));
	visualWorld->didRender(bind(&DidRenderCallback, _1, _2));
	visualWorld->background(make_shared<MaterialProperty>(CubeImageNamed("nebula1_blue", "png")));

	auto inputManager = make_shared<WindowInputManager>(window);

	auto scene = SceneNamed("cat_island/cat_island", SCENE_IMPORT_OPTIONS::IMPORT_GEOMETRIES
													 | SCENE_IMPORT_OPTIONS::IMPORT_MATERIALS
													 | SCENE_IMPORT_OPTIONS::IMPORT_CAMERAS);
	scene->visualWorld(visualWorld);
	scene->inputManager(inputManager);
	scene->debugOptions(DEBUG_OPTIONS::SHOW_STATS_OVERLAY);
	scene->update(bind(&UpdateCallback, _1, _2));

	auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, make_shared<Color>(0.2f, 0.2, 0.2, 1.0));
	ambientLight->name("ambient");
	auto ambientLightNode = Node::LightNode(ambientLight);
	ambientLightNode = ambientLightNode;
	scene->rootNode()->addChild(ambientLightNode);

	auto pointLight = make_shared<Light>(LIGHT_TYPE::POINT, Color::White());
	pointLight->name("point");
	pointLight->attenuationFactor(0.00005);
	pointLightNode = Node::LightNode(pointLight);
	scene->rootNode()->addChild(pointLightNode);
	auto materialProperty = make_shared<MaterialProperty>(pointLight->color());
	auto material = make_shared<Material>();
	material->name("LIGHT material");
	material->emissive(materialProperty);
	auto geometry = make_shared<Sphere>(1.5, 16);
	geometry->addMaterial(material);
	pointLightNode->geometry(geometry);

	if (ORTHO_CAMERA) {
		auto orthoCameraNode = Node::CameraNode(
				make_shared<OrthographicCamera>("Ortho camera", (AABB){{0, 0, 0},
																	   {100, 100, 100}}));
		scene->rootNode()->addChild(orthoCameraNode);
	}

	// random lights

//	{
//		const int NUM_RANDOM_LIGHTS = 64;
//		for (int l = 0; l < NUM_RANDOM_LIGHTS; ++l) {
//			auto light = make_shared<Light>(LIGHT_TYPE::POINT);
//			light->attenuationFactor(0.0001);
//			static const float yOffset = 30;
//			static const int range = 75;
//			auto lightNode = Node::LightNode(light);
//			int randX = Uniform(-range, range);
//			int randY = Uniform(-range, range);
//			int randZ = Uniform(-range, range);
//			lightNode->position(vec3(randX, randY + yOffset, randZ));
//			auto color = Color::Random();
//			light->color(color);
//
//			auto geometry = make_shared<Sphere>(1.5, 16);
//
//			auto materialProperty = make_shared<MaterialProperty>(color);
//			auto material = make_shared<Material>();
//			material->emissive(materialProperty);
//			geometry->addMaterial(material);
//			lightNode->geometry(geometry);
//
//			scene->rootNode()->addChild(lightNode);
//		}
//	}

	window->open();
	scene->run();

	return 0;
}

/***************************************************************************************
	Scene Callbacks
 ***************************************************************************************/

void UpdateCallback(Scene& scene, float time) {
	LOG_T(logger, "scene: {:p}, time: {}", (void*)&scene, time);

	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;

	auto window = static_pointer_cast<Window>(scene.visualWorld()->renderContext());

	// get input

	auto keysPressed = scene.inputManager()->keysPressed();
	auto keysDown = scene.inputManager()->keysDown();

	if (keysPressed.count(KEY::ESCAPE)) {
		window->close();
	}

	if (keysPressed.count(KEY::T)) {
		LOG_I(logger, "TREE:\n{}", StringFromTree(*(scene.rootNode())));
	}

	if 		(keysPressed.count(KEY::ONE))	SetAllFilterModes(FILTER_MODE::NEAREST, scene);
	else if (keysPressed.count(KEY::TWO))	SetAllFilterModes(FILTER_MODE::LINEAR, scene);
	else if (keysPressed.count(KEY::THREE))	SetAllFilterModes(FILTER_MODE::NEAREST_MIPMAP_NEAREST, scene);
	else if (keysPressed.count(KEY::FOUR))	SetAllFilterModes(FILTER_MODE::NEAREST_MIPMAP_LINEAR, scene);
	else if (keysPressed.count(KEY::FIVE))	SetAllFilterModes(FILTER_MODE::LINEAR_MIPMAP_NEAREST, scene);
	else if (keysPressed.count(KEY::SIX))	SetAllFilterModes(FILTER_MODE::LINEAR_MIPMAP_LINEAR, scene);

	if 		(keysPressed.count(KEY::LEFT_BRACKET))	SetAllMaxAnisotropy(1, scene);
	else if (keysPressed.count(KEY::RIGHT_BRACKET))	SetAllMaxAnisotropy(16, scene);

	if 		(keysPressed.count(KEY::F1)) 	pointLightNode->light()->attenuationFactor(0.0005);
	else if (keysPressed.count(KEY::F2)) 	pointLightNode->light()->attenuationFactor(0.00015);
	else if (keysPressed.count(KEY::F3)) 	pointLightNode->light()->attenuationFactor(0.00005);

	if (keysPressed.count(KEY::F)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
	}

	if (keysPressed.count(KEY::B)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
	}

	if (keysPressed.count(KEY::I)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
	}

	if (keysPressed.count(KEY::V)) {
		window->vSyncEnabled(!(window->vSyncEnabled()));
	}

	if (keysPressed.count(KEY::BACKSLASH)) {
		SaveSnapshot(*window);
	}

	if (keysPressed.count(KEY::R)) {
		if (!window->recordingGIF()) {
			StartGIFRecording(*window, 320, 8);
		}
		else {
			StopGIFRecording(*window);
		}
	}

	if (keysPressed.count(KEY::SLASH)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(KEY::FORWARD_DELETE)) {
		scene.paused(!scene.paused());
	}

	if (window->cursorCaptured()) {

		vec2 mousePositionDelta = scene.inputManager()->mousePositionDelta();

		// move camera

		auto pov = scene.visualWorld()->pointOfView();
		if (pov) {

			vec2 mouseScrollWheelDelta = scene.inputManager()->mouseScrollWheelDelta();
			if (mouseScrollWheelDelta.y) {

				static const float FOV_SPEED = 2.5; // degrees/roll

				shared_ptr<PerspectiveCamera> camera = static_pointer_cast<PerspectiveCamera>(pov->camera());
				auto fov = camera->yFov();
				fov += mouseScrollWheelDelta.y * -radians(FOV_SPEED);
				camera->yFov(fov);
			}

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

			auto keysDown = scene.inputManager()->keysDown();

			static float MOVE_SPEED = 0;
			if (!MOVE_SPEED) MOVE_SPEED = Max(scene.rootNode()->extent());

			float moveMultiplier = 1.0;
			if (keysDown.count(KEY::LEFT_CONTROL)) {
				moveMultiplier = 2.0;
			}

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

			if (keysDown.count(KEY::SPACE)) {
				float direction = 1;
				if (keysDown.count(KEY::LEFT_SHIFT)) {
					direction = -1;
				}
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camUp;
				pov->position(pov->position() + positionDelta * direction);
			}
		}
	}

	// move the light

	if (pointLightNode) {

		auto center = vec3(0, 30, 0);

		static auto extent = scene.rootNode()->extent();
		static float radius = std::max(std::max(extent.x, extent.y), extent.z) * .46;
		static float radiusX = radius;
		static float radiusY = radius;

		static float rotationSpeed = radians(30.0); // deg/secs
		static float angle = 0;
		angle += rotationSpeed * deltaSeconds;

		float x = sin(angle) * radiusX;
		float y = cos(angle) * radiusY;

		pointLightNode->position(center + vec3(x, y, -x));
	}
}

/***************************************************************************************
	VisualWorld Callbacks
 ***************************************************************************************/

void WillRenderCallback(VisualWorld& world, float time) {
	LOG_T(logger, "world: {:p}, time: {}", (void*)&world, time);
}

void DidRenderCallback(VisualWorld& world, float time) {
	LOG_T(logger, "world: {:p}, time: {}", (void*)&world, time);
}

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
