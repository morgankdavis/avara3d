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
constexpr bool					DARK =					false;


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
					  ? make_shared<MaterialProperty>(CubeImageNamed("belfast_sunset", "png"))
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

	const float PLANE_LENGTH = 30.0;
	const float PLANE_WIDTH = 30.0;
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
											  nullptr,
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



//
//	// add the palm tree
//
//	auto palmScene = SceneNamed("palm2/palm2", "obj");
//	auto palmNode = palmScene->rootNode();
//	palmNode->name("Palm node");
//	for (auto n : palmScene->rootNode()->children(true)) {
//		if (n->geometry()) {
//			for (auto m : n->geometry()->materials()) {
//				m->doubleSided(true);
//			}
//		}
//	}
//
//	auto palmPhysicsBody = PhysicsBody::StaticBody();
//	palmPhysicsBody->mass(0);
//	palmPhysicsBody->friction(1);
//	palmPhysicsBody->restitution(0.25);
//	palmNode->physicsBody(palmPhysicsBody);
//
//	scene->rootNode()->addChild(palmNode);
//
//
//



	{
//		auto testGeometry = GeometryNamed("rubberDuck/rubberDuck");
//		auto testGeometry = GeometryNamed("slurm/slurm");
//		auto testGeometry = GeometryNamed("slurm_colors/slurm_colors");
//		auto testGeometry = GeometryNamed("cardboardBox2/cardboardBox2");
//		auto testGeometry = GeometryNamed("palm1/palm1"); // multiple
//		auto testGeometry = GeometryNamed("palm2/palm2"); // single
//		auto testGeometry = GeometryNamed("island/Island");
//		auto testGeometry = GeometryNamed("teapot");
//		auto testGeometry = GeometryNamed("apple1_lod/apple1_lod");
//		auto testGeometry = GeometryNamed("banana_lod/banana_lod");
//		auto testGeometry = GeometryNamed("cherry1_lod/cherry1_lod");
//		auto testGeometry = GeometryNamed("ConvaliaBouquet");
//		auto testGeometry = GeometryNamed("dragon");
//		auto testGeometry = GeometryNamed("orange1_lod/orange1_lod");
//		auto testGeometry = GeometryNamed("pear_lod/pear_lod");
		auto testGeometry = GeometryNamed("pineapple_lod/pineapple_lod");
//		auto testGeometry = GeometryNamed("pallet_rot/pallet_rot");
//		auto testGeometry = GeometryNamed("pallet_rot/pallet_rot");
//		auto testGeometry = GeometryNamed("siamese/siamese");
//		auto testGeometry = GeometryNamed("tuna_rot/tuna_rot");
//		auto testGeometry = GeometryNamed("cartoon_palm_tree/cartoon_palm_tree");
//		auto testGeometry = GeometryNamed("crocus/crocus");

		for (auto &m: testGeometry->materials()) {
			m->doubleSided(true);
		}

		for (auto& e : testGeometry->elements()) {
			AE_LOG_D("verts: {}", e->vertices().size());
			AE_LOG_D("faces: {}", e->faces().size());
		}

//		AE_LOG_I("testGeometry.extent: {}", StringFromGLMVec3(testGeometry->extent()));

		auto testNode = Node::GeometryNode(testGeometry);
		testNode->position({0, 15, 0});

//		testNode->physicsBody(PhysicsBody::KinematicBody());
//		testNode->physicsBody()->shape()->type(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON);
//
//		auto shape = make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON, testNode->geometry().get());
//		auto body = make_shared<PhysicsBody>(PHYSICS_BODY_TYPE::STATIC, shape);
//		testNode->physicsBody(body);

		scene->rootNode()->addChild(testNode);
	}

//	{
//		auto testScene = SceneNamed("importTest");
//	auto testScene = SceneNamed("gltf_fast/importTest", "gltf");
		auto testScene = SceneNamed("pineapple_only", "glb");
//		auto testScene = SceneNamed("pineapple_only/pineapple_only", "gltf");

		for (auto& node : testScene->rootNode()->children()) {
			scene->rootNode()->addChild(node);
		}
//	}


	for (auto& node : scene->rootNode()->children(true)) {
		auto light = node->light();
		if (light) {
			AE_LOG_D("light: {}",
					 light->name().has_value()
					 ? *light->name() : "unnamed");

			if (light->type() == LIGHT_TYPE::POINT) {
				auto sphere = make_shared<Sphere>(0.25f, 12);
				auto property = make_shared<MaterialProperty>(light->color());
				auto material = make_shared<Material>(nullptr, nullptr, nullptr, property);
				sphere->addMaterial(material);
				node->geometry(sphere);
			}
		}

		auto camera = node->camera();
		if (camera) {
			AE_LOG_D("camera: {}",
					 camera->name().has_value()
					 ? *camera->name() : "unnamed");
		}
	}



//	static shared_ptr<Color> colors[] = {
//			Color::White(),
//			Color::Red(),
//			Color::Orange(),
//			Color::Yellow(),
//			Color::Lime(),
//			Color::Blue()
//	};
//	auto color = colors[Uniform(0, 5)];
//
//	constexpr float BALL_RADIUS = 0.55;
//	auto sphereGrometry = make_shared<Sphere>(BALL_RADIUS, 3);
//	auto ballNode = Node::GeometryNode(sphereGrometry);
//	auto diffuseProperty = make_shared<MaterialProperty>(color);
//	auto specularProperty = make_shared<MaterialProperty>(Color::White());
//	auto ballMaterial = make_shared<Material>(nullptr, diffuseProperty, specularProperty);
//	ballMaterial->specularExponent(125.0);
//	ballNode->geometry()->addMaterial(ballMaterial);
//	ballNode->position({0, 20, 0});
//
//	auto ballPhysicsBody = PhysicsBody::DynamicBody();
//	ballPhysicsBody->mass(0.2); // vollyball
//	ballPhysicsBody->restitution(1.0);
//	ballPhysicsBody->friction(0.015);
//	ballPhysicsBody->rollingFriction(0.15);
//
//	ballNode->physicsBody(ballPhysicsBody);
//
//	scene->rootNode()->addChild(ballNode);







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
