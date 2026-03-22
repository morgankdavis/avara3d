//
//  main.cpp
//  avara3d
//
//  Created by Morgan Davis on 12/02/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include <memory>
#include <utility>
#include <vector>

#include "a3d/a3d.h"
#include "a3d/physics/backend/bullet/BulletBodyProxy.h"
#include "a3d/util/filesystem.h"
#include "a3d/util/snapshot.h"
#include "a3d/util/string.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;
using namespace std::placeholders;

const Log::Level						APP_LOG_LEVEL		{Log::Level::Debug};
const uvec2								WINDOW_SIZE			{1280, 768};
const bool								FULLSCREEN			{false};
const bool								ENABLE_HIGH_DPI		{true};
const RenderContext::AntialiasingMode	ANTIALIAS_MODE		{RenderContext::AntialiasingMode::Msaa4X};
const bool								ENABLE_VSYNC		{false};
const bool								CAPTURE_CURSOR		{false};
const float								MOUSE_SENSITIVITY	{0.5};
const float								PHYSICS_TIMESTEP	{1.0/120.0};
const bool								DARK				{false};

void UpdateCallback(Scene& scene, double time, double deltaTime);
void WillRenderCallback(VisualWorld& world, double time, double deltaTime);
void DidRenderCallback(VisualWorld& world, double time, double deltaTime);
void DidSimulatePhysicsCallback(PhysicalWorld& world, double time, double deltaTime);

void InitLog();
void LogBuildInfo();

// https://stackoverflow.com/questions/66068134/segmentation-fault-when-using-a-shared-ptr-for-private-key
shared_ptr<Node>*			g_meshNode;
vector<shared_ptr<Mesh>>*	g_meshes;
shared_ptr<Mesh>*			g_mesh;

//Node*			g_meshNode;
//vector<Mesh*>	g_meshes;
//Mesh*			g_mesh;

int main(int argc, const char* argv[]) {

	using util::filesystem::MeshNamed;

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
	//	visualWorld->fogStartDistance(50.0);
	//	visualWorld->fogEndDistance(400.0);
	//	visualWorld->fogDensityExponent(1.0);
	//	visualWorld->fogColor(DARK ? Color::DarkGray() : Color::LightGray());
	//	auto background = DARK
	//					  ? make_shared<MaterialProperty>(Color::Black())
	//					          //make_shared<MaterialProperty>(CubeImageNamed("belfast_sunset", "png"))
	//					  : make_shared<MaterialProperty>(CubeImageNamed("kloppenheim", "png"));
		Material::Property background = monostate{};
		if (DARK) background = Color::Black();
		else background = make_shared<Texture>(util::filesystem::CubeImageNamed("kloppenheim", "png"));
		visualWorld->background(background);
		visualWorld->willRenderCallback(bind(&WillRenderCallback, _1, _2, _3));
		visualWorld->didRenderCallback(bind(&DidRenderCallback, _1, _2, _3));

		auto physicalWorld = make_unique<PhysicalWorld>();
		physicalWorld->timestep(PHYSICS_TIMESTEP);
		physicalWorld->didSimulateCallback(bind(&DidSimulatePhysicsCallback, _1, _2, _3));

		auto scene = make_unique<Scene>(std::move(visualWorld), std::move(physicalWorld), std::move(inputManager));
		scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);
		scene->updateCallback(bind(&UpdateCallback, _1, _2, _3));

	//	auto ambientColor = DARK
	//						? Color::LightGray()
	//						: make_shared<Color>(.85f);
		//auto ambientLight = make_shared<Light>(LightType::Ambient, Color::DarkGray());
		auto ambientLight = make_shared<AmbientLight>(Color::DarkGray());
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
		planeNode->mesh(Box::Mesh(PLANE_LENGTH, 0, PLANE_WIDTH));
		auto gridImage = DARK
				? util::filesystem::ImageNamed("grid10")->inverted()
				: util::filesystem::ImageNamed("grid10");
		auto planeTexture = make_shared<Texture>(std::move(gridImage));
		planeTexture->sampler()->wrapS(Sampler::WrapMode::Repeat);
		planeTexture->sampler()->wrapT(Sampler::WrapMode::Repeat);
		planeTexture->sampler()->maxAnisotropy(16);
		planeTexture->sampler()->minificationFilter(Sampler::FilterMode::LinearMipmapLinear);
		planeTexture->sampler()->magnificationFilter(Sampler::FilterMode::Linear);
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

		planeMaterial->uvScale(PLANE_LENGTH/10.0f);
		planeMaterial->doubleSided(false);
		planeNode->mesh()->addMaterial(planeMaterial);
	//	planeNode->mesh()->replaceMaterial(0, planeMaterial);
		//planeNode->rotation({1, 0, 0}, radians(3*90.0));
		planeNode->position({planeNode->position().x, 0, planeNode->position().z});



	//	auto planePhysicsBody = PhysicsBody::StaticBody();
	//	planeNode->physicsBody(planePhysicsBody);
	//	planePhysicsBody->friction(1);
	//	planePhysicsBody->restitution(0.25);

		scene->rootNode()->addChild(planeNode);





	//	{
			{
//				auto pointLight = make_shared<Light>(LightType::Point, Color::LightGray());
				auto pointLight = make_shared<PointLight>(Color::LightGray());
				//pointLight->attenuationFactor(0);
//				pointLight->attenuation(Attenuation{
//						.constant = 1.0f, .linear = 0.0f, .quadratic = 0.1f});
				auto pointLightNode = Node::LightNode(pointLight);
				pointLightNode->position({5, 5, 0});

				auto material = make_shared<Material>(monostate{},
													  monostate{},
													  monostate{},
													  Color::White());
				//auto sphere = Mesh::Sphere(0.1f, 12);
				auto sphere = Sphere::Mesh(0.1f, 12, material);

				//sphere->addMaterial(material);
	//			sphere->replaceMaterial(0, material);
				pointLightNode->mesh(sphere);

				scene->rootNode()->addChild(pointLightNode);
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







//		{
//			auto mesh = Box::Mesh(1.0f, 2.0f, 3.0f);
//			auto node = make_shared<Node>();
//			node->name("box");
//			node->mesh(mesh);
//			scene->rootNode()->addChild(node);
//			//node->position(vec3(0.0f, 1.0f, 0.0f));
//		}



		{
			auto mesh = Wedge::Mesh(1.0f, 5.0f, 10.0f);
			auto node = make_shared<Node>();
			mesh->name("wedge");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			//node->rotation({-1.0f, 0.0f, 0.0f}, radians(90.0f));
			node->position(vec3(0.0f, 5.0f, 0.0f));
		}

//		{
//			auto mesh = Sphere::Mesh(1.0f);
//			auto node = make_shared<Node>();
//			mesh->name("sphere");
//			node->mesh(mesh);
//			scene->rootNode()->addChild(node);
//			//node->position(vec3(5.0f, 2.5f, 0.0f));
//		}

//		{
//			auto mesh = Dome::Mesh(5.0f,
//								   0, math::radians(90.0),
//								   0, math::radians(180.0));
//			auto node = make_shared<Node>();
//			mesh->name("dome");
//			node->mesh(mesh);
//			scene->rootNode()->addChild(node);
//			node->position(vec3(5.0f, 2.5f, 0.0f));
//		}

//		{
//			auto mesh = Disk::Mesh(2.5f, 5.0f);
//			auto node = make_shared<Node>();
//			node->name("disk");
//			node->mesh(mesh);
//			scene->rootNode()->addChild(node);
//			node->position(vec3(0.0f, 5.0f, 0.0f));
//		}














		window->center();
		window->open();

		do {
			scene->update();
		} while (window->isOpen());

		return 0;


		const vector<string> meshNames = {
				"apple_lod/apple_lod",
				"banana_lod/banana_lod",
				"cardboard_box/cardboard_box",
				"cartoon_palm_tree/cartoon_palm_tree",
				"cherries_lod/cherries_lod",
				"crocus/crocus",
				"dragon/dragon",
				"island/island",
				"orange_lod/orange_lod",
				"pallet/pallet",
				"palm/palm",
				"palms/palms",
				"pear_lod/pear_lod",
				"pineapple_lod/pineapple_lod",
				"rubber_duck/rubber_duck",
				"siamese/siamese",
				"slurm/slurm",
				"teapot/teapot",
				"tuna/tuna"
		};

		vector<shared_ptr<Mesh>> meshes;
		meshes.reserve(meshNames.size());
		for (const auto& meshName: meshNames) {
			meshes.push_back(MeshNamed(meshName));
		}

		g_meshes = &meshes;
	//	g_meshes = vector<Mesh*>();
	//	g_meshes.reserve(meshes.size());
	//	for (auto& m : meshes) {
	//		g_meshes.push_back(m.get());
	//	}

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
		auto meshNode = shared_ptr(Node::MeshNode(mesh));
		meshNode->position({0, 5, 0});

		g_meshNode = &meshNode;
		scene->rootNode()->addChild(meshNode);
	//	g_meshNode = meshNode.get();


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

		window->center();
		window->open();

		do {
			scene->update();
		} while (window->isOpen());
	}
	catch (std::exception& e)
	{
		log::app::f()("Exception: {}", e.what());
		return -1;
	}

	return 0;
}


/// Scene Callbacks ///

void UpdateCallback(Scene& scene, double time, double deltaTime) {
	log::app::t()("scene: {:p}, time: {}, deltaTime: {}", (void*)&scene, time, deltaTime);

	GLFWWindow* window = nullptr;
	if (scene.visualWorld()) {
		window = dynamic_cast<GLFWWindow*>(scene.visualWorld()->renderContext());
	}


	// get input

	auto im = static_cast<DesktopInputManager*>(scene.inputManager());

	auto mouseButtonsDown = im->mouseButtonsDown();
	auto mouseButtonsPressed = im->mouseButtonsPressed();
	auto keysDown = im->keysDown();
	auto keysPressed = im->keysPressed();
	auto cursorCaptured = true;
	if (window) {
		cursorCaptured = window->cursorCaptured();
	}

	using Key = DesktopInputManager::Key;
	using MouseButton = DesktopInputManager::MouseButton;

	if (keysPressed.count(Key::Escape)) {
		window->close();
	}

	if (keysPressed.count(Key::T)) {
		log::app::i()("TREE:\n{}", util::string::TreeString(*(scene.rootNode())));
	}

	if (keysPressed.count(Key::One)) {

		//auto cameraNodes = vector<Node*>();
		auto cameraNodes = vector<std::shared_ptr<Node>>();
		for (auto& node : scene.rootNode()->children(true)) {
			auto camera = node->camera();
			if (camera) {
				cameraNodes.push_back(node);
			}
		}

		scene.visualWorld()->pointOfView(cameraNodes[0]);
	}
	if (keysPressed.count(Key::Two)) {

		auto cameraNodes = vector<std::shared_ptr<Node>>();
		for (auto& node : scene.rootNode()->children(true)) {
			auto camera = node->camera();
			if (camera) {
				cameraNodes.push_back(node);
			}
		}

		scene.visualWorld()->pointOfView(cameraNodes[1]);
	}
	if (keysPressed.count(Key::Three)) {

		auto cameraNodes = vector<std::shared_ptr<Node>>();
		for (auto& node : scene.rootNode()->children(true)) {
			auto camera = node->camera();
			if (camera) {
				cameraNodes.push_back(node);
			}
		}

		scene.visualWorld()->pointOfView(cameraNodes[2]);
	}



	static int index = 0;
	if (keysPressed.count(Key::LeftBracket)) {
		g_mesh = &((*g_meshes)[--index]);
		auto name = (*g_mesh)->name();
		if (name) log::app::d()("name: {}", *name);
		(*g_meshNode)->mesh(*g_mesh);
	}
	if (keysPressed.count(Key::RightBracket)) {
		g_mesh = &((*g_meshes)[++index]);
		auto name = (*g_mesh)->name();
		if (name) log::app::d()("name: {}", *name);
		//meshNode = Node::meshNode(mesh);
		(*g_meshNode)->mesh(*g_mesh);
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
	if (keysPressed.count(Key::P)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsBoundingBoxes)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
											   DebugOptions::ShowPhysicsBoundingBoxes));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(),
											DebugOptions::ShowPhysicsBoundingBoxes));
		}
	}
	if (keysPressed.count(Key::G)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
											   DebugOptions::ShowPhysicsWireframes));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(),
											DebugOptions::ShowPhysicsWireframes));
		}
	}
	if (keysPressed.count(Key::C)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsContactPoints)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
											   DebugOptions::ShowPhysicsContactPoints));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(),
											DebugOptions::ShowPhysicsContactPoints));
		}
	}
	if (keysPressed.count(Key::N)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsNormals)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
											   DebugOptions::ShowPhysicsNormals));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(),
											DebugOptions::ShowPhysicsNormals));
		}
	}

	if (keysPressed.count(Key::V)) {
		window->vSyncEnabled(!window->vSyncEnabled());
	}

	if (keysPressed.count(Key::Backslash)) {
		util::snapshot::SaveSnapshot(*window);
	}

	if (keysPressed.count(Key::Slash)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(Key::R)) {
		if (!window->recordingGIF()) {
			util::snapshot::StartGIFRecording(*window, {320, 240}, 8);
		}
		else {
			util::snapshot::StopGIFRecording(*window);
		}
	}

	if (cursorCaptured) {

		// mouselook

		vec2 mousePositionDelta = im->mousePositionDelta();

		if (auto pov = scene.visualWorld()->pointOfView().lock()) {

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

			static float MOVE_SPEED = 0;
			if (!MOVE_SPEED) MOVE_SPEED = math::max(scene.rootNode()->extent());

			float moveMultiplier = 1.0;
			if (keysDown.count(Key::LeftControl)) {
				moveMultiplier = 2.0;
			}

			if (keysDown.count(Key::W) || mouseButtonsDown.count(MouseButton::Four)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * moveMultiplier * camForward;
				pov->position(pov->position() + positionDelta);
			}
			else if (keysDown.count(Key::S)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * moveMultiplier * -camForward;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(Key::A)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * moveMultiplier * -camRight;
				pov->position(pov->position() + positionDelta);
			}
			else if (keysDown.count(Key::D)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * moveMultiplier * camRight;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(Key::Space)) {
				float direction = 1;
				if (keysDown.count(Key::LeftShift)) {
					direction = -1;
				}
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * moveMultiplier * camUp;
				pov->position(pov->position() + positionDelta * direction);
			}
		}
	}
}

/// VisualWorld Callbacks ///

void WillRenderCallback(VisualWorld& world, double time, double deltaTime) {
	log::app::t()("world: {:p}, time: {}, deltaTime: {}", (void*)&world, time, deltaTime);
}

void DidRenderCallback(VisualWorld& world, double time, double deltaTime) {
	log::app::t()("world: {:p}, time: {}, deltaTime: {}", (void*)&world, time, deltaTime);
}

/// PhysicalWorld Callbacks ///

void DidSimulatePhysicsCallback(PhysicalWorld& world, double time, double deltaTime) {
	log::app::t()("world: {:p}, time: {}, deltaTime: {}", (void*)&world, time, deltaTime);
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
	auto version = buildInfo.version();
	log::app::i()("A3D version: {}.{}.{}", version.major, version.minor, version.patch);
	log::app::i()("Build: {}", buildInfo.number());
	log::app::i()("Type: {}", buildInfo.type() == BuildInfo::Type::Debug ? "Debug" : "Release");
	log::app::i()("Origin: {}", buildInfo.origin() == BuildInfo::Origin::CI ? "CI" : "AdHoc");
}
