//
//  main.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/02/23.
//  Copyright © 2023 Morgan K Davis. All rights reserved.
//

#include <algorithm>
#include <memory>
#include <vector>

#include "glm/glm.hpp"

#include "ae/ae.h"
#include "ae/Utilities.h"
#include "ae/physics/bullet/BulletBodyProxy.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;
using namespace std::placeholders;


void UpdateCallback(Scene& scene, float time);
void WillRenderCallback(VisualWorld& world, float time);
void DidRenderCallback(VisualWorld& world, float time);
void DidSimulatePhysicsCallback(PhysicalWorld& world, float time);


constexpr bool					USE_HIGH_DPI =			false;
constexpr unsigned				WINDOW_WIDTH =			1280;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr ANTIALIASING_MODE		MSAA_MODE =				ANTIALIASING_MODE::MSAA_4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr float					MOUSE_SENSITIVITY =		0.5;
constexpr float					PHYSICS_TIMESTEP =		1.0/120.0;
constexpr bool					DARK =					true;


std::shared_ptr<ae::Logger>		logger;


int main(int argc, const char* argv[]) {

	logger = make_shared<Logger>("sandbox", Logger::MainLogger()->sinks());

	auto buildInfo = BuildInfo::Info();
	auto version = buildInfo.version();
	LOG_I(logger, "AE version: {}.{}.{}",
		  version.major, version.minor, version.patch);
	LOG_I(logger, "Build: {}", buildInfo.number());
	LOG_I(logger, "Type: {}",
		  buildInfo.type() == BuildInfo::TYPE::DEBUG ? "DEBUG" : "RELEASE");
	LOG_I(logger, "Origin: {}",
		  buildInfo.origin() == BuildInfo::ORIGIN::CI ? "CI" : "ADHOC");
	auto time = buildInfo.time();

	auto window = make_shared<Window>(RENDER_API::OPENGL,
									  FULLSCREEN,
									  WINDOW_WIDTH,
									  WINDOW_HEIGHT,
									  USE_HIGH_DPI,
									  MSAA_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_shared<VisualWorld>(window);
//	visualWorld->fogStartDistance(50.0);
//	visualWorld->fogEndDistance(400.0);
//	visualWorld->fogDensityExponent(1.0);
//	visualWorld->fogColor(DARK ? Color::DarkGray() : Color::LightGray());
	auto background = DARK
					  ? make_shared<MaterialProperty>(Color::Black())
					          //make_shared<MaterialProperty>(CubeImageNamed("belfast_sunset", "png"))
					  : make_shared<MaterialProperty>(CubeImageNamed("kloppenheim", "png"));
	visualWorld->background(background);
	visualWorld->willRender(bind(&WillRenderCallback, _1, _2));
	visualWorld->didRender(bind(&DidRenderCallback, _1, _2));

	auto physicalWorld = make_shared<PhysicalWorld>();
	physicalWorld->timestep(PHYSICS_TIMESTEP);
	physicalWorld->didSimulate(bind(&DidSimulatePhysicsCallback, _1, _2));

	auto inputManager = make_shared<WindowInputManager>(window);

	auto scene = make_shared<Scene>(visualWorld, physicalWorld, inputManager);
	scene->debugOptions(DEBUG_OPTIONS::SHOW_STATS_OVERLAY);
	scene->update(bind(&UpdateCallback, _1, _2));

//	auto ambientColor = DARK
//						? Color::LightGray()
//						: make_shared<Color>(.85f);
	auto ambientColor = Color::DarkGray();
	auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, ambientColor);
	auto ambientLightNode = Node::LightNode(ambientLight);
	scene->rootNode()->addChild(ambientLightNode);

//	auto pointColor = DARK
//					  ? Color::LightGray()
//					  : Color::Gray();
//	auto pointLight = make_shared<Light>(LIGHT_TYPE::POINT, pointColor);
//	pointLight->attenuationFactor(0);
//	auto pointLightNode = Node::LightNode(pointLight);
//	pointLightNode->position(vec3(35, 20, (DARK ? -1.0 : -1.0 ) * 52) * vec3(2.5, 2.5, 2.5));
//	scene->rootNode()->addChild(pointLightNode);





	// ground plane

	const float PLANE_LENGTH = 20.0;
	const float PLANE_WIDTH = 20.0;
	auto planeNode = make_shared<Node>("Ground plane node");
	planeNode->geometry(make_shared<Box>(PLANE_LENGTH, PLANE_WIDTH, 0));
	auto gridImage = DARK ? ImageNamed("grid10")->inverted() : ImageNamed("grid10");
	auto planeMaterialProperty = make_shared<MaterialProperty>(gridImage);
	planeMaterialProperty->wrapS(WRAP_MODE::REPEAT);
	planeMaterialProperty->wrapT(WRAP_MODE::REPEAT);
	planeMaterialProperty->maxAnisotropy(16);
	planeMaterialProperty->minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR);
	planeMaterialProperty->magnificationFilter(FILTER_MODE::LINEAR);
	shared_ptr<Material> planeMaterial = nullptr;
	if (DARK) {
		planeMaterial = make_shared<Material>(nullptr,
											  nullptr,
											  make_shared<MaterialProperty>(Color::White()),
											  planeMaterialProperty);
	}
	else {
		planeMaterial = make_shared<Material>(nullptr,
											  planeMaterialProperty,
											  nullptr);//make_shared<MaterialProperty>(make_shared<Color>(.1f)));
	}

	planeMaterial->uvScale(PLANE_LENGTH/10.0);
	planeMaterial->doubleSided(false);
	planeNode->geometry()->addMaterial(planeMaterial);
	planeNode->rotation({1, 0, 0}, radians(3*90.0));
	planeNode->position({planeNode->position().x, 0, planeNode->position().z});

	auto planePhysicsBody = PhysicsBody::StaticBody();
//	AE_LOG_I("planeNode t: {}", StringFromGLMMat4(planeNode->transform()));
//	AE_LOG_I("planeNode wt: {}", StringFromGLMMat4(planeNode->worldTransform()));
	planeNode->physicsBody(planePhysicsBody);
	planePhysicsBody->friction(1);
	planePhysicsBody->restitution(0.25);

	scene->rootNode()->addChild(planeNode);





//	{
//		{
//			auto pointLight = make_shared<Light>(LIGHT_TYPE::POINT, Color::LightGray());
//			pointLight->attenuationFactor(0);
//			auto pointLightNode = Node::LightNode(pointLight);
//			pointLightNode->position({5, 5, 0});
//			scene->rootNode()->addChild(pointLightNode);
//
//			auto sphere = make_shared<Sphere>(0.1f, 12);
//			auto property = make_shared<MaterialProperty>(pointLight->color());
//			auto material = make_shared<Material>(nullptr, nullptr, nullptr, property);
//			sphere->addMaterial(material);
//			pointLightNode->geometry(sphere);
//		}
//
////		auto testGeometry = GeometryNamed("rubber_duck/rubber_duck");
////		auto testGeometry = GeometryNamed("slurm/slurm");
////		auto testGeometry = GeometryNamed("cardboard_box/cardboard_box");
////		auto testGeometry = GeometryNamed("palm/palm");
////		auto testGeometry = GeometryNamed("palms/palms");
////		auto testGeometry = GeometryNamed("island/island");
////		auto testGeometry = GeometryNamed("teapot");
////		auto testGeometry = GeometryNamed("apple_lod/apple_lod");
////		auto testGeometry = GeometryNamed("banana_lod/banana_lod");
////		auto testGeometry = GeometryNamed("cherries_lod/cherries_lod");
//
//// 		REVISIT ME
////			- no normals
////			- normals
////			- no groupings (1 element?)
////			- groupings (multiple elements?)
////			- textures
////		auto testGeometry = GeometryNamed("convalia_bouquet");
//
////		auto testGeometry = GeometryNamed("dragon");
////		auto testGeometry = GeometryNamed("orange_lod/orange_lod");
////		auto testGeometry = GeometryNamed("pear_lod/pear_lod");
////		auto testGeometry = GeometryNamed("pineapple_lod/pineapple_lod");
//
////		REVISIT ME
////		auto testGeometry = GeometryNamed("pallet_rot/pallet_rot");
//
////		auto testGeometry = GeometryNamed("siamese/siamese");
////		auto testGeometry = GeometryNamed("tuna_rot/tuna_rot");
////		auto testGeometry = GeometryNamed("cartoon_palm_tree/cartoon_palm_tree");
//		auto testGeometry = GeometryNamed("crocus/crocus");
//
//		for (auto &m: testGeometry->materials()) {
//			m->doubleSided(true);
//		}
//		auto testNode = Node::GeometryNode(testGeometry);
//		testNode->position({0, 5, 0});
//
//		scene->rootNode()->addChild(testNode);
//	}



	{
		auto testScene = SceneNamed("import_test/import_test");
//		auto testScene = SceneNamed("import_test", "glb");
//		auto testScene = SceneNamed("khr_gltf2_samples/ABeautifulGame/glTF/ABeautifulGame");
//		auto testScene = SceneNamed("khr_gltf2_samples/BarramundiFish/glTF/BarramundiFish");
//		auto testScene = SceneNamed("khr_gltf2_samples/Duck/glTF/Duck"); // STRIDE
		auto importRoot = testScene->rootNode();
		auto rootPos = importRoot->position();
		importRoot->position({rootPos.x, rootPos.y+2, rootPos.z});
		scene->rootNode()->addChild(importRoot);

//		{
//			auto pointLight = make_shared<Light>(LIGHT_TYPE::POINT, Color::LightGray());
//			pointLight->attenuationFactor(0);
//			auto pointLightNode = Node::LightNode(pointLight);
//			pointLightNode->position({5, 5, 0});
//			scene->rootNode()->addChild(pointLightNode);
//
//			auto sphere = make_shared<Sphere>(0.1f, 12);
//			auto property = make_shared<MaterialProperty>(pointLight->color());
//			auto material = make_shared<Material>(nullptr, nullptr, nullptr, property);
//			sphere->addMaterial(material);
//			pointLightNode->geometry(sphere);
//		}

		for (auto& node : scene->rootNode()->children(true)) {
			auto light = node->light();
			if (light) {
				if (light->type() == LIGHT_TYPE::POINT) {
					auto sphere = make_shared<Sphere>(0.1f, 12);
					auto property = make_shared<MaterialProperty>(light->color());
					auto material = make_shared<Material>(nullptr, nullptr, nullptr, property);
					sphere->addMaterial(material);
					node->geometry(sphere);
				}
			}
		}
	}




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

	auto inputManager = scene.inputManager();

	shared_ptr<Window> window = nullptr;
	if (scene.visualWorld()) {
		window = static_pointer_cast<Window>(scene.visualWorld()->renderContext());
	}




	// get input

	auto mouseButtonsDown = inputManager->mouseButtonsDown();
	auto mouseButtonsPressed = inputManager->mouseButtonsPressed();
	auto keysDown = inputManager->keysDown();
	auto keysPressed = inputManager->keysPressed();
	auto cursorCaptured = true;
	if (window) {
		cursorCaptured = window->cursorCaptured();
	}

	if (keysPressed.count(KEY::ESCAPE)) {
		window->close();
	}

	if (keysPressed.count(KEY::FORWARD_DELETE)) {
		scene.paused(!scene.paused());
	}

	if (keysPressed.count(KEY::T)) {
		LOG_I(logger, "TREE:\n{}", StringFromTree(*(scene.rootNode())));
	}

	if (keysPressed.count(KEY::ONE)) {

		auto cameraNodes = vector<shared_ptr<Node>>();
		for (auto& node : scene.rootNode()->children(true)) {
			auto camera = node->camera();
			if (camera) {
				cameraNodes.push_back(node);
			}
		}

		scene.visualWorld()->pointOfView(cameraNodes[0]);
	}
	if (keysPressed.count(KEY::TWO)) {

		auto cameraNodes = vector<shared_ptr<Node>>();
		for (auto& node : scene.rootNode()->children(true)) {
			auto camera = node->camera();
			if (camera) {
				cameraNodes.push_back(node);
			}
		}

		scene.visualWorld()->pointOfView(cameraNodes[1]);
	}
	if (keysPressed.count(KEY::THREE)) {

		auto cameraNodes = vector<shared_ptr<Node>>();
		for (auto& node : scene.rootNode()->children(true)) {
			auto camera = node->camera();
			if (camera) {
				cameraNodes.push_back(node);
			}
		}

		scene.visualWorld()->pointOfView(cameraNodes[2]);
	}


	if (keysPressed.count(KEY::F)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(),
													DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(),
												 DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
	}
	if (keysPressed.count(KEY::B)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(),
													DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(),
												 DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
	}
	if (keysPressed.count(KEY::I)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(),
													DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(),
												 DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
	}
	if (keysPressed.count(KEY::P)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(),
													DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(),
												 DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES));
		}
	}
	if (keysPressed.count(KEY::G)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(),
													DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(),
												 DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES));
		}
	}
	if (keysPressed.count(KEY::C)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(),
													DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(),
												 DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS));
		}
	}
	if (keysPressed.count(KEY::N)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(),
													DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(),
												 DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS));
		}
	}

	if (keysPressed.count(KEY::V)) {
		window->vSyncEnabled(!window->vSyncEnabled());
	}

	if (keysPressed.count(KEY::BACKSLASH)) {
		SaveSnapshot(*window);
	}

	if (keysPressed.count(KEY::SLASH)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(KEY::R)) {
		if (!window->recordingGIF()) {
			StartGIFRecording(*window, 320, 8);
		}
		else {
			StopGIFRecording(*window);
		}
	}

	if (cursorCaptured) {

		// mouselook

		vec2 mousePositionDelta = inputManager->mousePositionDelta();

		auto pov = scene.visualWorld()->pointOfView();
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

			static float MOVE_SPEED = 0;
			if (!MOVE_SPEED) MOVE_SPEED = Max(scene.rootNode()->extent());

			float moveMultiplier = 1.0;
			if (keysDown.count(KEY::LEFT_CONTROL)) {
				moveMultiplier = 2.0;
			}

			if (keysDown.count(KEY::W) || mouseButtonsDown.count(MOUSE_BUTTON::FOUR)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camForward;
				pov->position(pov->position() + positionDelta);
			}
			else if (keysDown.count(KEY::S)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * -camForward;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(KEY::A)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * -camRight;
				pov->position(pov->position() + positionDelta);
			}
			else if (keysDown.count(KEY::D)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camRight;
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
	PhysicalWorld Callbacks
 ***************************************************************************************/

void DidSimulatePhysicsCallback(PhysicalWorld& world, float time) {
	LOG_T(logger, "world: {:p}, time: {}", (void*)&world, time);
}
