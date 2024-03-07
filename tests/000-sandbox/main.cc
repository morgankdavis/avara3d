//
//  main.cpp
//	avara3d
//
//  Created by Morgan Davis on 12/02/23.
//  Copyright © 2023 Morgan K Davis. All rights reserved.
//

#include <algorithm>
#include <memory>
#include <vector>

#include "glm/glm.hpp"

#include "a3d/a3d.h"
#include "a3d/Utilities.h"
#include "a3d/physics/bullet/BulletBodyProxy.h"


using namespace a3d;
using namespace a3d::utils;
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
constexpr AntialiasingMode		MSAA_MODE =				AntialiasingMode::Msaa4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr float					MOUSE_SENSITIVITY =		0.5;
constexpr float					PHYSICS_TIMESTEP =		1.0/120.0;
constexpr bool					DARK =					true;


std::shared_ptr<a3d::Logger>	logger;


shared_ptr<Node>				meshNode;
shared_ptr<Mesh> 				geometry;
vector<shared_ptr<Mesh>> 		meshes;


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

	auto window = make_shared<Window>(RenderingApi::OpenGL,
									  *utils::ExecutableName(),
									  WINDOW_WIDTH,
									  WINDOW_HEIGHT,
									  FULLSCREEN,
									  USE_HIGH_DPI,
									  MSAA_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_shared<VisualWorld>(window);
//	visualWorld->fogStartDistance(50.0);
//	visualWorld->fogEndDistance(400.0);
//	visualWorld->fogDensityExponent(1.0);
//	visualWorld->fogColor(DARK ? Color::DarkGray() : Color::LightGray());
//	auto background = DARK
//					  ? make_shared<MaterialProperty>(Color::Black())
//					          //make_shared<MaterialProperty>(CubeImageNamed("belfast_sunset", "png"))
//					  : make_shared<MaterialProperty>(CubeImageNamed("kloppenheim", "png"));
	MaterialProperty background = monostate{};
	if (DARK) background = Color::Black();
	else background = make_shared<Texture>(CubeImageNamed("kloppenheim", "png"));
	visualWorld->background(background);
	visualWorld->willRender(bind(&WillRenderCallback, _1, _2));
	visualWorld->didRender(bind(&DidRenderCallback, _1, _2));

	auto physicalWorld = make_shared<PhysicalWorld>();
	physicalWorld->timestep(PHYSICS_TIMESTEP);
	physicalWorld->didSimulate(bind(&DidSimulatePhysicsCallback, _1, _2));

	auto inputManager = make_shared<WindowInputManager>(window);

	auto scene = make_shared<Scene>(visualWorld, physicalWorld, inputManager);
	scene->debugOptions(DebugOptions::ShowStatsOverlay);
	scene->update(bind(&UpdateCallback, _1, _2));

//	auto ambientColor = DARK
//						? Color::LightGray()
//						: make_shared<Color>(.85f);
	auto ambientColor = Color::DarkGray();
	auto ambientLight = make_shared<Light>(LightType::Ambient, ambientColor);
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
	//planeNode->mesh(Mesh::Box(PLANE_LENGTH, PLANE_WIDTH, 0));
	planeNode->mesh(Box::Mesh(PLANE_LENGTH, PLANE_WIDTH, 0));
	auto gridImage = DARK ? ImageNamed("grid10")->inverted() : ImageNamed("grid10");
	auto planeTexture = make_shared<Texture>(gridImage);
	planeTexture->sampler()->wrapS(WrapMode::Repeat);
	planeTexture->sampler()->wrapT(WrapMode::Repeat);
	planeTexture->sampler()->maxAnisotropy(16);
	planeTexture->sampler()->minificationFilter(FilterMode::LinearMipmapLinear);
	planeTexture->sampler()->magnificationFilter(FilterMode::Linear);
	shared_ptr<Material> planeMaterial = nullptr;
	if (DARK) {
		planeMaterial = make_shared<Material>(monostate{},
											  monostate{},
											  Color::White(),
											  planeTexture);
	}
	else {
		planeMaterial = make_shared<Material>(monostate{},
											  planeTexture,
											  monostate{});
	}

	planeMaterial->uvScale(PLANE_LENGTH/10.0);
	planeMaterial->doubleSided(false);
	planeNode->mesh()->addMaterial(planeMaterial);
//	planeNode->mesh()->replaceMaterial(0, planeMaterial);
	planeNode->rotation({1, 0, 0}, radians(3*90.0));
	planeNode->position({planeNode->position().x, 0, planeNode->position().z});

//	auto planePhysicsBody = PhysicsBody::StaticBody();
//	planeNode->physicsBody(planePhysicsBody);
//	planePhysicsBody->friction(1);
//	planePhysicsBody->restitution(0.25);

	scene->rootNode()->addChild(planeNode);





//	{
		{
			auto pointLight = make_shared<Light>(LightType::Point, Color::LightGray());
			pointLight->attenuationFactor(0);
			auto pointLightNode = Node::LightNode(pointLight);
			pointLightNode->position({5, 5, 0});
			scene->rootNode()->addChild(pointLightNode);

			auto material = make_shared<Material>(monostate{},
												  monostate{},
												  monostate{},
												  pointLight->color());
			//auto sphere = Mesh::Sphere(0.1f, 12);
			auto sphere = Sphere::Mesh(0.1f, 12, material);

			//sphere->addMaterial(material);
//			sphere->replaceMaterial(0, material);
			pointLightNode->mesh(sphere);
		}
//
//		auto testMesh = MeshNamed("rubber_duck/rubber_duck");
//		auto testMesh = MeshNamed("slurm/slurm");
//		auto testMesh = MeshNamed("cardboard_box/cardboard_box");
//		auto testMesh = MeshNamed("palm/palm");
//		auto testMesh = MeshNamed("palms/palms");
//		auto testMesh = MeshNamed("island/island");
//		auto testMesh = MeshNamed("teapot");
//		auto testMesh = MeshNamed("apple_lod/apple_lod");
//		auto testMesh = MeshNamed("banana_lod/banana_lod");
//		auto testMesh = MeshNamed("cherries_lod/cherries_lod");
// 		REVISIT ME
//			- no normals
//			- normals
//			- no groupings (1 element?)
//			- groupings (multiple elements?)
//			- textures
//		auto testMesh = MeshNamed("convalia_bouquet");
//		auto testMesh = MeshNamed("dragon");
//		auto testMesh = MeshNamed("orange_lod/orange_lod");
//		auto testMesh = MeshNamed("pear_lod/pear_lod");
//		auto testMesh = MeshNamed("pineapple_lod/pineapple_lod");
//		auto testMesh = MeshNamed("pallet/pallet");
//		auto testMesh = MeshNamed("siamese/siamese");
//		auto testMesh = MeshNamed("tuna_rot/tuna_rot");
//		auto testMesh = MeshNamed("cartoon_palm_tree/cartoon_palm_tree");
//		auto testMesh = MeshNamed("crocus/crocus");


	meshes = vector<shared_ptr<Mesh>>{
			MeshNamed("apple_lod/apple_lod"),
			MeshNamed("banana_lod/banana_lod"),
			MeshNamed("cardboard_box/cardboard_box"),
			MeshNamed("cartoon_palm_tree/cartoon_palm_tree"),
			MeshNamed("cherries_lod/cherries_lod"),
			MeshNamed("crocus/crocus"),
			MeshNamed("dragon/dragon"),
			MeshNamed("island/island"),
			MeshNamed("orange_lod/orange_lod"),
			MeshNamed("pallet/pallet"),
			MeshNamed("palm/palm"),
			MeshNamed("palms/palms"),
			MeshNamed("pear_lod/pear_lod"),
			MeshNamed("pineapple_lod/pineapple_lod"),
			MeshNamed("rubber_duck/rubber_duck"),
			MeshNamed("siamese/siamese"),
			MeshNamed("slurm/slurm"),
			MeshNamed("teapot/teapot"),
			MeshNamed("tuna/tuna")
		};

//	auto testMesh = MeshNamed("apple_lod/apple_lod");
//	auto testMesh = MeshNamed("banana_lod/banana_lod");
//	auto testMesh = MeshNamed("cardboard_box/cardboard_box");
//	auto testMesh = MeshNamed("cartoon_palm_tree/cartoon_palm_tree");
//	auto testMesh = MeshNamed("cherries_lod/cherries_lod");
//	auto testMesh = MeshNamed("crocus/crocus");
//	auto testMesh = MeshNamed("dragon/dragon");
//	auto testMesh = MeshNamed("island/island");
//	auto testMesh = MeshNamed("orange_lod/orange_lod");
//	auto testMesh = MeshNamed("pallet/pallet");
//	auto testMesh = MeshNamed("palm/palm");
//	auto testMesh = MeshNamed("palms/palms");
//	auto testMesh = MeshNamed("pear_lod/pear_lod");
//	auto testMesh = MeshNamed("pineapple_lod/pineapple_lod");
//	auto testMesh = MeshNamed("rubber_duck/rubber_duck");
//	auto testMesh = MeshNamed("siamese/siamese");
//	auto testMesh = MeshNamed("slurm/slurm");
//	auto testMesh = MeshNamed("teapot/teapot");
//	auto testMesh = MeshNamed("tuna/tuna");
	// -> 19


	auto mesh = meshes[0];
	meshNode = Node::MeshNode(mesh);
	meshNode->position({0, 5, 0});
	scene->rootNode()->addChild(meshNode);


//		for (auto &m: testMesh->materials()) {
//			m->doubleSided(true);
//		}
//		auto testNode = Node::meshNode(testMesh);
////		testNode->position({0, 5, 0});
//		scene->rootNode()->addChild(testNode);
////	}



////	{
//		auto testScene = SceneNamed("import_test/import_test");
////		auto testScene = SceneNamed("rubber_duck_gltf/rubber_duck",
////									SCENE_IMPORT_OPTIONS::IMPORT_MESHES
////									| SCENE_IMPORT_OPTIONS::IMPORT_MATERIALS
////									| SCENE_IMPORT_OPTIONS::FIRST_MESH_ONLY);
////		auto testScene = SceneNamed("import_test", "glb");
////		auto testScene = SceneNamed("khr_gltf2_samples/ABeautifulGame/glTF/ABeautifulGame");
////		auto testScene = SceneNamed("khr_gltf2_samples/BarramundiFish/glTF/BarramundiFish");
////		auto testScene = SceneNamed("khr_gltf2_samples/Duck/glTF/Duck"); // STRIDE
//
//		auto importRoot = testScene->rootNode();
//		auto rootPos = importRoot->position();
//		importRoot->position({rootPos.x, rootPos.y+2, rootPos.z});
//		scene->rootNode()->addChild(importRoot);
////	}

//
////		{
////			auto pointLight = make_shared<Light>(LIGHT_TYPE::POINT, Color::LightGray());
////			pointLight->attenuationFactor(0);
////			auto pointLightNode = Node::LightNode(pointLight);
////			pointLightNode->position({5, 5, 0});
////			scene->rootNode()->addChild(pointLightNode);
////
////			auto sphere = make_shared<Sphere>(0.1f, 12);
////			auto property = make_shared<MaterialProperty>(pointLight->color());
////			auto material = make_shared<Material>(nullptr, nullptr, nullptr, property);
////			sphere->addMaterial(material);
////			pointLightNode->mesh(sphere);
////		}
//
//		for (auto& node : scene->rootNode()->children(true)) {
//			auto light = node->light();
//			if (light) {
//				if (light->type() == LIGHT_TYPE::POINT) {
//					auto sphere = make_shared<Sphere>(0.1f, 12);
//					auto property = make_shared<MaterialProperty>(light->color());
//					auto material = make_shared<Material>(nullptr, nullptr, nullptr, property);
//					sphere->addMaterial(material);
//					node->mesh(sphere);
//				}
//			}
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

	if (keysPressed.count(Key::Escape)) {
		window->close();
	}

	if (keysPressed.count(Key::ForwardDelete)) {
		scene.paused(!scene.paused());
	}

	if (keysPressed.count(Key::T)) {
		LOG_I(logger, "TREE:\n{}", StringFromTree(*(scene.rootNode())));
	}

	if (keysPressed.count(Key::One)) {

		auto cameraNodes = vector<shared_ptr<Node>>();
		for (auto& node : scene.rootNode()->children(true)) {
			auto camera = node->camera();
			if (camera) {
				cameraNodes.push_back(node);
			}
		}

		scene.visualWorld()->pointOfView(cameraNodes[0]);
	}
	if (keysPressed.count(Key::Two)) {

		auto cameraNodes = vector<shared_ptr<Node>>();
		for (auto& node : scene.rootNode()->children(true)) {
			auto camera = node->camera();
			if (camera) {
				cameraNodes.push_back(node);
			}
		}

		scene.visualWorld()->pointOfView(cameraNodes[1]);
	}
	if (keysPressed.count(Key::Three)) {

		auto cameraNodes = vector<shared_ptr<Node>>();
		for (auto& node : scene.rootNode()->children(true)) {
			auto camera = node->camera();
			if (camera) {
				cameraNodes.push_back(node);
			}
		}

		scene.visualWorld()->pointOfView(cameraNodes[2]);
	}





//	auto it = find(geometries.begin(), geometries.end(), geometry);
//	int x = std::distance(geometries, it);
	static int index = 0;
	if (keysPressed.count(Key::LeftBracket)) {
		geometry = meshes[--index];
		auto name = geometry->name();
		if (name) A3D_LOG_D("name: {}", *name);
		//meshNode = Node::meshNode(geometry);
		meshNode->mesh(geometry);
	}
	if (keysPressed.count(Key::RightBracket)) {
		geometry = meshes[++index];
		auto name = geometry->name();
		if (name) A3D_LOG_D("name: {}", *name);
		//meshNode = Node::meshNode(geometry);
		meshNode->mesh(geometry);
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
	if (keysPressed.count(Key::P)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsBoundingBoxes)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(),
											   DebugOptions::ShowPhysicsBoundingBoxes));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(),
											DebugOptions::ShowPhysicsBoundingBoxes));
		}
	}
	if (keysPressed.count(Key::G)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(),
											   DebugOptions::ShowPhysicsWireframes));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(),
											DebugOptions::ShowPhysicsWireframes));
		}
	}
	if (keysPressed.count(Key::C)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsContactPoints)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(),
											   DebugOptions::ShowPhysicsContactPoints));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(),
											DebugOptions::ShowPhysicsContactPoints));
		}
	}
	if (keysPressed.count(Key::N)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsNormals)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(),
											   DebugOptions::ShowPhysicsNormals));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(),
											DebugOptions::ShowPhysicsNormals));
		}
	}

	if (keysPressed.count(Key::V)) {
		window->vSyncEnabled(!window->vSyncEnabled());
	}

	if (keysPressed.count(Key::Backslash)) {
		SaveSnapshot(*window);
	}

	if (keysPressed.count(Key::Slash)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(Key::R)) {
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
			if (keysDown.count(Key::LeftControl)) {
				moveMultiplier = 2.0;
			}

			if (keysDown.count(Key::W) || mouseButtonsDown.count(MouseButton::Four)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camForward;
				pov->position(pov->position() + positionDelta);
			}
			else if (keysDown.count(Key::S)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * -camForward;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(Key::A)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * -camRight;
				pov->position(pov->position() + positionDelta);
			}
			else if (keysDown.count(Key::D)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * moveMultiplier * camRight;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(Key::Space)) {
				float direction = 1;
				if (keysDown.count(Key::LeftShift)) {
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
