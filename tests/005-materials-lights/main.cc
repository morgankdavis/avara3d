//
//  main.cpp
//  avara3d
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include <iostream>
#include <memory>
#include <vector>
#include <utility>

#include "glm/glm.hpp"

#include "a3d/a3d.h"
#include "a3d/Utilities.h"


using namespace a3d;
using namespace glm;
using namespace std;
using namespace std::placeholders;


constexpr LogLevel				LOG_LEVEL =				LogLevel::Debug;
constexpr uvec2					WINDOW_SIZE =			{1280, 768};
constexpr bool					FULLSCREEN =			false;
constexpr bool					ENABLE_HIGH_DPI =		true;
constexpr AntialiasingMode		ANTIALIAS_MODE =		AntialiasingMode::Msaa4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr bool 					ORTHO_CAMERA =			false;
constexpr float					MOUSE_SENSITIVITY =		0.5;


void UpdateCallback(Scene& scene, float time, float deltaTime);
void WillRenderCallback(VisualWorld& world, float time, float deltaTime);
void DidRenderCallback(VisualWorld& world, float time, float deltaTime);


void InitLog();
void LogBuildInfo();
void SetAllFilterModes(FilterMode mode, Scene& scene);
void SetAllMaxAnisotropy(float anisotropy, Scene& scene);
void ProcessEdit(Node& node, set<Key>& keysDown, set<Key>& keysPressed);


std::unique_ptr<a3d::Logger>	g_logger;
a3d::Node*						g_pointLightNode;


int main(int argc, const char* argv[]) {

	try {
		InitLog();
		LogBuildInfo();

		auto window = make_unique<Window>(RenderingApi::OpenGL,
										  *utils::ExecutableName(),
										  WINDOW_SIZE,
										  FULLSCREEN,
										  ENABLE_HIGH_DPI,
										  ANTIALIAS_MODE);
		window->vSyncEnabled(ENABLE_VSYNC);
		window->cursorCaptured(CAPTURE_CURSOR);

		auto visualWorld = make_unique<VisualWorld>(*window);
		visualWorld->fogStartDistance(500.0);
		visualWorld->fogEndDistance(5000.0);
		visualWorld->fogDensityExponent(1.0);
		visualWorld->fogColor(Color::LightGray());
		visualWorld->willRender(bind(&WillRenderCallback, _1, _2, _3));
		visualWorld->didRender(bind(&DidRenderCallback, _1, _2, _3));
		visualWorld->background(make_shared<Texture>(std::move(utils::CubeImageNamed("nebula1_blue", "png"))));

		auto inputManager = make_unique<WindowInputManager>(window.get());

		auto scene = utils::SceneNamed("cat_island/cat_island", SceneImportOptions::ImportMeshes
														 | SceneImportOptions::ImportMaterials
														 | SceneImportOptions::ImportCameras);

		scene->visualWorld(std::move(visualWorld));
		scene->inputManager(std::move(inputManager));
		scene->debugOptions(DebugOptions::ShowStatsOverlay);
		scene->update(bind(&UpdateCallback, _1, _2, _3));

		auto ambientLight = make_shared<Light>(LightType::Ambient, make_unique<Color>(0.2f, 0.2, 0.2, 1.0));
		ambientLight->name("ambient");
		auto ambientLightNode = Node::LightNode(ambientLight);
		scene->rootNode()->addChild(ambientLightNode);

		auto pointLight = make_shared<Light>(LightType::Point, Color::White());
		pointLight->name("point");
		pointLight->attenuationFactor(0.00005);
		auto pointLightNode = Node::LightNode(pointLight);
		g_pointLightNode = pointLightNode.get(); // <- how is this not crashing?
		auto material = make_shared<Material>();
		material->name("LIGHT material");
		material->emission(Color::White());
		auto geometry = Sphere::Mesh(1.5, 4, material);
		pointLightNode->mesh(geometry);
		scene->rootNode()->addChild(pointLightNode);


		// test emissive property
	//	for (auto& node : scene->rootNode()->children(true)) {
	//		if (node->mesh()) {
	//			for (auto& material : node->mesh()->materials()) {
	//				if (!holds_alternative<std::monostate>(material->diffuse())) {
	//					material->emission(material->diffuse());
	//				}
	//			}
	//		}
	//	}


		if (ORTHO_CAMERA) {
			auto orthoCameraNode = Node::CameraNode(
					make_shared<OrthographicCamera>("Ortho camera", (AABB){{0, 0, 0},
																		   {100, 100, 100}}));
			scene->rootNode()->addChild(orthoCameraNode);
		}

	//	auto siameseNode = scene->rootNode()->childNamed("Siamese");
	//	siameseNode->mesh()->firstMaterial()->fillMode(FillMode::Lines); // works

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

		window->center();
		window->open();
		scene->run();
	}
	catch (NoAvailableMiceException& e)
	{
		// on macOS 10.15 Catalina+, this is probably a permissions issue,
		// and the OS will alert the user.  just quit nicely.
		LOG_F(g_logger, "No available mice.");
		return -1;
	}
	catch (Exception& e)
	{
		LOG_F(g_logger, "Exception: {}", e.what());
		return -1;
	}

	return 0;
}

/***************************************************************************************
	Scene Callbacks
 ***************************************************************************************/

void UpdateCallback(Scene& scene, float time, float deltaTime) {
	LOG_T(g_logger, "scene: {:p}, time: {}, deltaTime: {}", (void*)&scene, time, deltaTime);

	auto window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());

	// get input

	auto keysPressed = scene.inputManager()->keysPressed();
	auto keysDown = scene.inputManager()->keysDown();

	if (keysPressed.count(Key::Escape)) {
		window->close();
	}

	if (keysPressed.count(Key::T)) {
		LOG_I(g_logger, "TREE:\n{}", utils::StringFromTree(*(scene.rootNode())));
	}

	if 		(keysPressed.count(Key::One))	SetAllFilterModes(FilterMode::Nearest, scene);
	else if (keysPressed.count(Key::Two))	SetAllFilterModes(FilterMode::Linear, scene);
	else if (keysPressed.count(Key::Three))	SetAllFilterModes(FilterMode::NearestMipmapNearest, scene);
	else if (keysPressed.count(Key::Four))	SetAllFilterModes(FilterMode::NearestMipmapLinear, scene);
	else if (keysPressed.count(Key::Five))	SetAllFilterModes(FilterMode::LinearMipmapNearest, scene);
	else if (keysPressed.count(Key::Six))	SetAllFilterModes(FilterMode::LinearMipmapLinear, scene);

	if 		(keysPressed.count(Key::LeftBracket))	SetAllMaxAnisotropy(1, scene);
	else if (keysPressed.count(Key::RightBracket))	SetAllMaxAnisotropy(16, scene);

	if 		(keysPressed.count(Key::F1)) 	g_pointLightNode->light()->attenuationFactor(0.0005);
	else if (keysPressed.count(Key::F2)) 	g_pointLightNode->light()->attenuationFactor(0.00015);
	else if (keysPressed.count(Key::F3)) 	g_pointLightNode->light()->attenuationFactor(0.00005);

	if (keysPressed.count(Key::F)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowWireframes)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(), DebugOptions::ShowWireframes));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(), DebugOptions::ShowWireframes));
		}
	}

	if (keysPressed.count(Key::B)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowBoundingBoxes)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
		}
	}

	if (keysPressed.count(Key::I)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowStatsOverlay)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
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

	if (keysPressed.count(Key::Slash)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(Key::ForwardDelete)) {
		scene.paused(!scene.paused());
	}

	if (window->cursorCaptured()) {

		vec2 mousePositionDelta = scene.inputManager()->mousePositionDelta();

		// move camera

		if (auto pov = scene.visualWorld()->pointOfView().lock()) {

			vec2 mouseScrollWheelDelta = scene.inputManager()->mouseScrollWheelDelta();
			if (mouseScrollWheelDelta.y) {

				static const float FOV_SPEED = 2.5; // degrees/roll

				auto camera = dynamic_pointer_cast<PerspectiveCamera>(pov->camera());
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
			if (!MOVE_SPEED) MOVE_SPEED = utils::Max(scene.rootNode()->extent());

			float moveMultiplier = 1.0;
			if (keysDown.count(Key::LeftControl)) {
				moveMultiplier = 2.0;
			}

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

			if (keysDown.count(Key::Space)) {
				float direction = 1;
				if (keysDown.count(Key::LeftShift)) {
					direction = -1;
				}
				vec3 positionDelta = deltaTime * MOVE_SPEED * moveMultiplier * camUp;
				pov->position(pov->position() + positionDelta * direction);
			}
		}
	}

	// move the light

	if (g_pointLightNode) {

		auto center = vec3(0, 30, 0);

		static auto extent = scene.rootNode()->extent();
		static float radius = std::max(std::max(extent.x, extent.y), extent.z) * .46;
		static float radiusX = radius;
		static float radiusY = radius;

		static float rotationSpeed = radians(30.0); // deg/secs
		static float angle = 0;
		angle += rotationSpeed * deltaTime;

		float x = sin(angle) * radiusX;
		float y = cos(angle) * radiusY;

		g_pointLightNode->position(center + vec3(x, y, -x));
	}
}

/***************************************************************************************
	VisualWorld Callbacks
 ***************************************************************************************/

void WillRenderCallback(VisualWorld& world, float time, float deltaTime) {
	LOG_T(g_logger, "world: {:p}, time: {}, deltaTime: {}", (void*)&world, time, deltaTime);
}

void DidRenderCallback(VisualWorld& world, float time, float deltaTime) {
	LOG_T(g_logger, "world: {:p}, time: {}, deltaTime: {}", (void*)&world, time, deltaTime);
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

	g_logger = make_unique<Logger>(executableName, std::move(sinks));
	g_logger->level(LOG_LEVEL);

	Logger::MainLogger().level(LOG_LEVEL);
}

void LogBuildInfo() {

	auto buildInfo = BuildInfo::Info();
	auto version = buildInfo.version();
	LOG_I(g_logger, "A3D version: {}.{}.{}", version.major, version.minor, version.patch);
	LOG_I(g_logger, "Build: {}", buildInfo.number());
	LOG_I(g_logger, "Type: {}", buildInfo.type() == BuildInfo::Type::Debug ? "Debug" : "Release");
	LOG_I(g_logger, "Origin: {}", buildInfo.origin() == BuildInfo::Origin::CI ? "CI" : "AdHoc");
}

void SetAllFilterModes(FilterMode mode, Scene& scene) {

	LOG_I(g_logger, "SetAllFilterModes: {}", (unsigned)mode);

	for (auto& node : scene.rootNode()->children(true)) {

		auto geometry = node->mesh();
		if (geometry) {

			for (auto& material : geometry->materials()) {

				for (auto& [property, type] : material->properties()) {

					if (holds_alternative<shared_ptr<Texture>>(*property)) {
						auto texture = get<shared_ptr<Texture>>(*property);
						auto sampler = texture->sampler();
						sampler->minificationFilter(mode);
						sampler->magnificationFilter(mode);
					}
				}
			}
		}
	}
}

void SetAllMaxAnisotropy(float anisotropy, Scene& scene) {

	LOG_I(g_logger, "SetAllMaxAnisotropy: {}", anisotropy);

	for (auto& node : scene.rootNode()->children(true)) {

		auto geometry = node->mesh();
		if (geometry) {

			for (auto& material : geometry->materials()) {

				for (auto& [property, type] : material->properties()) {

					if (holds_alternative<shared_ptr<Texture>>(*property)) {
						auto texture = get<shared_ptr<Texture>>(*property);
						auto sampler = texture->sampler();
						sampler->maxAnisotropy(anisotropy);
					}
				}
			}
		}
	}
}
