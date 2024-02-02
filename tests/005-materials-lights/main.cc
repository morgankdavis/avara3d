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
std::shared_ptr<ae::Node>		ambientLightNode;
std::shared_ptr<ae::Node>		pointLightNode;
std::shared_ptr<ae::Node>		palletNode;
std::shared_ptr<ae::Node>		siameseNode;
std::shared_ptr<ae::Node>		palmsNode;

Node* 							editingNode;

//glm::vec3 editPosition;
//glm::vec3 editScale;
//glm::vec3 editEulerAngles;

//glm::vec3 editPositionModifier;
//float editScaleModifier;




int main(int argc, const char* argv[]) {

	logger = make_shared<Logger>("test-005", Logger::MainLogger()->sinks());
	logger->level(LOG_LEVEL::DEBUG);
	Logger::MainLogger()->level(LOG_LEVEL::DEBUG);
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

	auto scene = make_shared<Scene>(visualWorld, nullptr, inputManager);
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
	pointLightNode->position(vec3(50.0, 50.0, 50.0));
	scene->rootNode()->addChild(pointLightNode);
	pointLightNode->position(vec3(0.0, 0.0, 0.0));
	pointLightNode = pointLightNode;
	auto materialProperty = make_shared<MaterialProperty>(pointLight->color());
	auto material = make_shared<Material>();
	material->name("LIGHT material");
	material->emissive(materialProperty);
	auto geometry = make_shared<Sphere>(3.5, 16);
	geometry->addMaterial(material);
	pointLightNode->geometry(geometry);

	if (ORTHO_CAMERA) {
		auto orthoCameraNode = Node::CameraNode(
				make_shared<OrthographicCamera>("Ortho camera", (AABB){vec3{0, 0, 0},
																	   vec3{100, 100, 100}}));
		scene->rootNode()->addChild(orthoCameraNode);
	}

	siameseNode = Node::GeometryNode(GeometryNamed("siamese/siamese"));
	siameseNode->scale(siameseNode->scale() * 6.97f);
	siameseNode->position(vec3(-13.5, -64.5, 0));
	scene->rootNode()->addChild(siameseNode);

	auto islandNode = Node::GeometryNode(GeometryNamed("island/island"));
	islandNode->position(vec3(0.0f, -150.0f, 0.0f));
	scene->rootNode()->addChild(islandNode);

	palletNode = Node::GeometryNode(GeometryNamed("pallet/pallet"));
	palletNode->position({-65.5, -63, -3.5});
	palletNode->scale({24.2792, 24.2792, 24.2792});
	palletNode->eulerAngles({3.14158, 2.52807, -1.22138});
	auto palletSpecularProperty = make_shared<MaterialProperty>(Color::DarkGray());
	palletNode->geometry()->firstMaterial()->specular(palletSpecularProperty);
	scene->rootNode()->addChild(palletNode);

	auto tunaNode = Node::GeometryNode(GeometryNamed("tuna/tuna"));
	tunaNode->position({-10.5, -71.25, 20});
	tunaNode->scale({4.1521, 4.1521, 4.1521});
	tunaNode->eulerAngles({8.63111e-05, -0.523707, 1.5702});
	scene->rootNode()->addChild(tunaNode);

	palmsNode =Node::GeometryNode(GeometryNamed("palms/palms"));
	palmsNode->position(vec3(0.0f, -72.0f, 0.0f));
	palmsNode->scale(palmsNode->scale() * 2.5f);
	palmsNode->rotation({0.0f, 1.0f, 0.0f}, radians(-5.0f));
	scene->rootNode()->addChild(palmsNode);

	for (auto n : palmsNode->children(true)) {
		if (n->geometry()) {
			for (auto m : n->geometry()->materials()) {
				m->doubleSided(true);
			}
		}
	}


	// random lights

//	{
//		const int NUM_RANDOM_LIGHTS = 100;
//		for (int l = 0; l < NUM_RANDOM_LIGHTS; ++l) {
//			auto light = make_shared<Light>(LIGHT_TYPE::POINT);
//			light->attenuationFactor(0.0001);
//			auto lightNode = Node::LightNode(light);
//			int randX = Uniform(-150, 150);
//			int randY = Uniform(-150, 150);
//			int randZ = Uniform(-150, 150);
//			lightNode->position(vec3(randX, randY, randZ));
//			auto color = Color::Random();
//			light->color(color);
//			cout << "Adding random light with position: "
//				 << lightNode->position() << ", color: " << *light->color() << endl;
//
//			auto geometry = make_shared<Sphere>(3.5, 16);
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



	ProcessEdit(*editingNode, keysDown, keysPressed);



//	static auto oldScaleFactor = newScaleFactor;
//	if (keysPressed.count(KEY::UP)) {
////		ogNode = siameseNode.get();
////		ogScale = siameseNode->scale();
//		newScaleFactor += 0.025;
//	}
//	else if (keysPressed.count(KEY::DOWN)) {
//		newScaleFactor -= 0.025;
//	}
//	else if (keysPressed.count(KEY::LEFT)) {
//		newScaleFactor -= 1.0;
//	}
//	else if (keysPressed.count(KEY::RIGHT)) {
//		newScaleFactor += 1.0;
//	}
//	if (newScaleFactor != oldScaleFactor) {
//		editNode->scale(editScale * newScaleFactor);
//		AE_LOG_D("NEW SCALE FACTOR: {}", newScaleFactor);
//		AE_LOG_D("NEW SCALE: {}", utils::StringFromGLMVec3(editNode->scale()));
//		oldScaleFactor = newScaleFactor;
//	}




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

//	if 		(keysPressed.count(KEY::LEFT_BRACKET))	SetAllMaxAnisotropy(1, scene);
//	else if (keysPressed.count(KEY::RIGHT_BRACKET))	SetAllMaxAnisotropy(16, scene);

	if 		(keysPressed.count(KEY::F10)) 	ambientLightNode->light()->color(make_shared<Color>(0.1f, 0.1, 0.1, 1.0));
	else if (keysPressed.count(KEY::F11)) 	ambientLightNode->light()->color(make_shared<Color>(0.2f, 0.2, 0.2, 1.0));
	else if (keysPressed.count(KEY::F12)) 	ambientLightNode->light()->color(make_shared<Color>(0.3f, 0.3, 0.3, 1.0));

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

	if (keysPressed.count(KEY::U)) {
		siameseNode->removeFromParent();
	}

	if (keysPressed.count(KEY::O)) {
		siameseNode = nullptr;
	}

//	if (keysPressed.count(Key_M)) {
//		unsigned total = 0;
//		unsigned used = 0;
//		_window->getVRAMStats(total, used);
//	}

//	if (keysPressed.count(KEY::DEL)) {
//		for (auto n : _window->scene()->rootNode()->children(true)) {
//			n->geometry(nullptr);
//		}
//	}


	if (keysPressed.count(KEY::FORWARD_DELETE)) {
		palletNode->geometry(nullptr);
	}

	if (keysPressed.count(KEY::END)) {
		siameseNode->geometry(nullptr);
	}

	if (keysPressed.count(KEY::PAGE_DOWN)) {
		for (auto& n : palmsNode->children(true)) {
			n->geometry(nullptr);
		}
	}

//	for (auto& key : keysPressed) {
//		LOG_D(_logger, "KEY: {}", static_cast<int>(key));
//	}

	if (keysPressed.count(KEY::HOME)) {
		auto squirrelImage = ImageNamed("squirrel2");
		siameseNode->geometry()->firstMaterial()->diffuse()->contents(squirrelImage);
	}

	if (keysPressed.count(KEY::PAGE_UP)) {
		siameseNode->geometry(palletNode->geometry());
	}

//	if (keysPressed.count(KEY::ZERO)) {
//		auto teapot = SceneNamed("teapot", "dae");
//		siameseNode->geometry(teapot->rootNode()->children(false)[1]->geometry());
//	}

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

			//cout << "Camera distance: " << length(_cameraNode->position()) << endl;

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

		auto center = vec3(0, -75, 0);

		static float radiusX = 100.0;
		static float radiusY = 100.0;

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

void ProcessEdit(Node& node, set<KEY>& keysDown, set<KEY>& keysPressed) {

	if (editingNode) {
		bool fine = keysDown.count(KEY::LEFT_SHIFT);

		// *** position ***

		vec3 positionDelta = {};

		if (keysPressed.count(KEY::UP)) {
			positionDelta.z += (fine ? 0.25 : 5.0);
		}
		else if (keysPressed.count(KEY::DOWN)) {
			positionDelta.z -= (fine ? 0.25 : 5.0);
		}
		else if (keysPressed.count(KEY::LEFT)) {
			positionDelta.x -= (fine ? 0.25 : 5.0);
		}
		else if (keysPressed.count(KEY::RIGHT)) {
			positionDelta.x += (fine ? 0.25 : 5.0);
		}
		else if (keysPressed.count(KEY::ZERO)) {
			positionDelta.y += (fine ? 0.25 : 5.0);
		}
		else if (keysPressed.count(KEY::NINE)) {
			positionDelta.y -= (fine ? 0.25 : 5.0);
		}

		// *** scale ***

		vec3 scaleDelta = {1.0, 1.0, 1.0};

		if (keysPressed.count(KEY::EQUAL)) { // +
			auto delta = (fine ? .0025 : .01f);
			scaleDelta.x += delta;
			scaleDelta.y += delta;
			scaleDelta.z += delta;
		}
		else if (keysPressed.count(KEY::MINUS)) { // -
			auto delta = (fine ? .0025 : .01f);
			scaleDelta.x -= delta;
			scaleDelta.y -= delta;
			scaleDelta.z -= delta;
		}

		// *** rotate ***
		vec3 rotationDelta = {};

		if (keysPressed.count(KEY::COMMA)) { // <
			rotationDelta.y += radians(fine ? 1.0 : 5.0);
		}
		else if (keysPressed.count(KEY::PERIOD)) { // >
			rotationDelta.y -= radians(fine ? 1.0 : 5.0);
		}
		else if (keysPressed.count(KEY::LEFT_BRACKET)) { // [
			rotationDelta.z += radians(fine ? 1.0 : 5.0);
		}
		else if (keysPressed.count(KEY::RIGHT_BRACKET)) { // ]
			rotationDelta.z -= radians(fine ? 1.0 : 5.0);
		}
		else if (keysPressed.count(KEY::L)) {
			rotationDelta.x += radians(fine ? 1.0 : 5.0);
		}
		else if (keysPressed.count(KEY::P)) {
			rotationDelta.x -= radians(fine ? 1.0 : 5.0);
		}


		if (!utils::Zero(positionDelta, 0.0001)) {
			node.position(node.position() + positionDelta);
			AE_LOG_D("POSITION: {}", utils::StringFromGLMVec3(node.position()));
		}
		if (!utils::Equal(scaleDelta, {1.0, 1.0, 1.0}, 0.0001)) {
			node.scale(node.scale() * scaleDelta);
			AE_LOG_D("SCALE: {}", utils::StringFromGLMVec3(node.scale()));
		}
		if (!utils::Zero(rotationDelta, 0.0001)) {
			node.eulerAngles(node.eulerAngles() + rotationDelta);
			AE_LOG_D("ROTATION: {}", utils::StringFromGLMVec3(node.eulerAngles()));
		}

//		if (keysPressed.count(KEY::Q)) {
//			AE_LOG_D("POSITION: {}", utils::StringFromGLMVec3(node.position()));
//			AE_LOG_D("SCALE: {}", utils::StringFromGLMVec3(node.scale()));
//			AE_LOG_D("ROTATION: {}", utils::StringFromGLMVec3(node.eulerAngles()));
//		}
	}
}