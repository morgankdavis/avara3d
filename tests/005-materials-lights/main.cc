//
//  main.cpp
//  avara3d
//
//  Created by Morgan Davis on 11/19/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include <memory>
#include <vector>
#include <utility>

#include "a3d/a3d.h"
#include "a3d/Math.h"
#include "a3d/util/chrono.h"
#include "a3d/util/filesystem.h"
#include "a3d/util/flow.h"
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
const bool 								ORTHO_CAMERA		{false};
const float								MOUSE_SENSITIVITY	{0.5};

void UpdateCallback(Scene& scene, double time, double deltaTime);
void WillRenderCallback(VisualWorld& world, double time, double deltaTime);
void DidRenderCallback(VisualWorld& world, double time, double deltaTime);

void InitLog();
void LogBuildInfo();
void SetAllFilterModes(Sampler::FilterMode mode, Scene& scene);
void SetAllMaxAnisotropy(float anisotropy, Scene& scene);

a3d::Node*						g_pointLightNode;
double 							g_startTime;

int main(int argc, const char* argv[]) {

	try {
		g_startTime = util::chrono::Time();

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
		//visualWorld->usesDefaultLighting(true);
		visualWorld->willRenderCallback(bind(&WillRenderCallback, _1, _2, _3));
		visualWorld->didRenderCallback(bind(&DidRenderCallback, _1, _2, _3));
		visualWorld->background(make_shared<Texture>(std::move(util::filesystem::CubeImageNamed("nebula1_blue", "png"))));

		auto scene = util::filesystem::SceneNamed("cat_island/cat_island",
												  Scene::ImportOptions::ImportMeshes
												  | Scene::ImportOptions::ImportMaterials
												  | Scene::ImportOptions::ImportCameras);

		scene->visualWorld(std::move(visualWorld));
		scene->inputManager(std::move(inputManager));
		scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);
		scene->updateCallback(bind(&UpdateCallback, _1, _2, _3));

		auto ambientLight = make_shared<AmbientLight>(make_shared<Color>(0.1f));
		ambientLight->name("ambient");
		auto ambientLightNode = Node::LightNode(ambientLight);
		scene->rootNode()->addChild(ambientLightNode);

		auto pointLight = make_shared<PointLight>(Color::White());
		pointLight->name("point");
		pointLight->quadraticAttenuation(0.002);
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
//			AABB frustum = {{-100, -100, .01},
//							{100, 100, 1000}};
//			AABB frustum = {{0, 0, .01},
//							{100, 100, 1000}};
//			AABB frustum = {{-50, -50, .01},
//							{50, 50, 1000}};
			auto frustum = scene->rootNode()->aabb();
			frustum.min.z = 0.01;
			frustum.max.z = 10000;
			auto orthoCameraNode = Node::CameraNode(make_shared<OrthographicCamera>("Ortho camera", frustum));

			scene->rootNode()->addChild(orthoCameraNode);

			// find the imported camera node, get its world transform, and apply it to our ortho camera
			for (auto& node : scene->rootNode()->children(true)) {
				if (node->camera()) {
					orthoCameraNode->transform(node->worldTransform());
					auto pos = orthoCameraNode->position();
					pos.y = 0;
					orthoCameraNode->position(pos);
					scene->visualWorld()->pointOfView(orthoCameraNode);
					break;
				}
			}
		}

	//	auto siameseNode = scene->rootNode()->childNamed("Siamese");
	//	siameseNode->mesh()->firstMaterial()->fillMode(FillMode::Lines); // works

		// random lights

//		{
//			const int NUM_RANDOM_LIGHTS = 64;
//			for (int l = 0; l < NUM_RANDOM_LIGHTS; ++l) {
//				auto light = make_shared<Light>(LightType::Point);
//				light->attenuationFactor(0.0001);
//				static const float yOffset = 30;
//				static const int range = 75;
//				auto lightNode = Node::LightNode(light);
//				int randX = utils::Uniform(-range, range);
//				int randY = utils::Uniform(-range, range);
//				int randZ = utils::Uniform(-range, range);
//				lightNode->position(vec3(randX, randY + yOffset, randZ));
//				auto color = Color::Random();
//				light->color(color);
//
//				auto geometry = make_shared<Sphere>(1.5, 16);
//
////				auto materialProperty = make_shared<MaterialProperty>(color);
////				auto material = make_shared<Material>();
////				material->emissive(materialProperty);
////				geometry->addMaterial(material);
////				lightNode->geometry(geometry);
//
//				scene->rootNode()->addChild(lightNode);
//			}
//		}

		//auto extent = scene->rootNode()->extent();

		window->center();
		window->open();

		do {
			scene->update();
		} while (window->isOpen());
	}
	catch (Exception& e)
	{
		log::app::f()("Exception: {}", e.what());
		return -1;
	}

	return 0;
}

/// Scene Callbacks ///

void UpdateCallback(Scene& scene, double time, double deltaTime) {

	util::flow::on(2, [&] {
		double time = util::chrono::Time() - g_startTime;
		log::app::i()("START TIME: {}", time);
	});

	log::app::t()("scene: {:p}, time: {}, deltaTime: {}", (void*)&scene, time, deltaTime);

	auto window = dynamic_cast<GLFWWindow*>(scene.visualWorld()->renderContext());

	// get input

	auto im = static_cast<DesktopInputManager*>(scene.inputManager());
	auto keysPressed = im->keysPressed();
	auto keysDown = im->keysDown();

	using Key = DesktopInputManager::Key;
	using MouseButton = DesktopInputManager::MouseButton;

	if (keysPressed.count(Key::Escape)) {
		window->close();
	}

	if (keysPressed.count(Key::T)) {
		log::app::i()("TREE:\n{}", util::string::TreeString(*(scene.rootNode())));
	}

	using FilterMode = Sampler::FilterMode;
	if 		(keysPressed.count(Key::One))	SetAllFilterModes(FilterMode::Nearest, scene);
	else if (keysPressed.count(Key::Two))	SetAllFilterModes(FilterMode::Linear, scene);
	else if (keysPressed.count(Key::Three))	SetAllFilterModes(FilterMode::NearestMipmapNearest, scene);
	else if (keysPressed.count(Key::Four))	SetAllFilterModes(FilterMode::NearestMipmapLinear, scene);
	else if (keysPressed.count(Key::Five))	SetAllFilterModes(FilterMode::LinearMipmapNearest, scene);
	else if (keysPressed.count(Key::Six))	SetAllFilterModes(FilterMode::LinearMipmapLinear, scene);

	if 		(keysPressed.count(Key::LeftBracket))	SetAllMaxAnisotropy(1, scene);
	else if (keysPressed.count(Key::RightBracket))	SetAllMaxAnisotropy(16, scene);

//	if (auto attenuatedLight = dynamic_cast<AttenuatedLight*>(g_pointLightNode->light().get())) {
//		if (keysPressed.count(Key::F1)) attenuatedLight->constantAttenuation(1.0 - 0.0005);
//		else if (keysPressed.count(Key::F2)) attenuatedLight->constantAttenuation(1.0 - 0.00015);
//		else if (keysPressed.count(Key::F3)) attenuatedLight->constantAttenuation(1.0 - 0.00005);
//	}

	using DebugOptions = Scene::DebugOptions;

	if (keysPressed.count(Key::F)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowWireframes)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowWireframes));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowWireframes));
		}
	}

	if (keysPressed.count(Key::B)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowBoundingBoxes)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
		}
	}

	if (keysPressed.count(Key::I)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowStatsOverlay)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
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

	if (keysPressed.count(Key::Slash)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (window->cursorCaptured()) {

		vec2 mousePositionDelta = im->mousePositionDelta();

		// move camera

		if (auto pov = scene.visualWorld()->pointOfView().lock()) {

			vec2 mouseScrollWheelDelta = im->mouseScrollWheelDelta();
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

			float deltaRotX = math::atan(MOUSE_SPEED * mousePositionDelta.x);
			float deltaRotY = math::atan(MOUSE_SPEED * mousePositionDelta.y);

			vec3 angles = pov->eulerAngles();
			pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));

			// move

//			auto keysDown = im->keysDown();

			static float MOVE_SPEED = 0;
			if (!MOVE_SPEED) MOVE_SPEED = math::max(scene.rootNode()->extent());

			float moveMultiplier = 1.0;
			if (keysDown.count(Key::LeftControl)) {
				moveMultiplier = 2.0;
			}

			if(keysDown.count(Key::W)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * camForward;
				pov->position(pov->position() + positionDelta);
			}
			else if(keysDown.count(Key::S)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * -camForward;
				pov->position(pov->position() + positionDelta);
			}

			if(keysDown.count(Key::A)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * -camRight;
				pov->position(pov->position() + positionDelta);
			}
			else if(keysDown.count(Key::D)) {
				vec3 positionDelta = (float)deltaTime * MOVE_SPEED * camRight;
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

	// move the light

	if (g_pointLightNode) {

		auto center = vec3(0, 30, 0);

		static auto extent = scene.rootNode()->extent();
		//static float radius = std::max(std::max(extent.x, extent.y), extent.z) * .46;
		static float radius = math::max(extent) * .46; // a3d::math
		static float radiusX = radius;
		static float radiusY = radius;

		static float rotationSpeed = radians(30.0); // deg/secs
		static float angle = 0;
		angle += rotationSpeed * deltaTime;

		float x = math::sin(angle) * radiusX;
		float y = math::cos(angle) * radiusY;

		g_pointLightNode->position(center + vec3(x, y, -x));
	}
}

/// VisualWorld Callbacks ///

void WillRenderCallback(VisualWorld& world, double time, double deltaTime) {
	log::app::t()("world: {:p}, time: {}, deltaTime: {}", (void*)&world, time, deltaTime);
}

void DidRenderCallback(VisualWorld& world, double time, double deltaTime) {
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

void SetAllFilterModes(Sampler::FilterMode mode, Scene& scene) {

	log::app::i()("SetAllFilterModes: {}", (unsigned)mode);

	for (auto& node : scene.rootNode()->children(true)) {

		auto geometry = node->mesh();
		if (geometry) {

			for (auto& material : geometry->materials()) {

				for (auto& [property, type] : material->properties()) {

					if (auto texture = get_if<shared_ptr<Texture>>(property)) {
						auto sampler = (*texture)->sampler();
						sampler->minificationFilter(mode);
						sampler->magnificationFilter(mode);
					}

//					if (holds_alternative<shared_ptr<Texture>>(*property)) {
//						auto texture = get<shared_ptr<Texture>>(*property);
//						auto sampler = texture->sampler();
//						sampler->minificationFilter(mode);
//						sampler->magnificationFilter(mode);
//					}
				}
			}
		}
	}
}

void SetAllMaxAnisotropy(float anisotropy, Scene& scene) {

	log::app::i()("SetAllMaxAnisotropy: {}", anisotropy);

	for (auto& node : scene.rootNode()->children(true)) {

		auto geometry = node->mesh();
		if (geometry) {

			for (auto& material : geometry->materials()) {

				for (auto& [property, type] : material->properties()) {

					if (auto texture = get_if<shared_ptr<Texture>>(property)) {
						auto sampler = (*texture)->sampler();
						sampler->maxAnisotropy(anisotropy);
					}

//					if (holds_alternative<shared_ptr<Texture>>(*property)) {
//						auto texture = get<shared_ptr<Texture>>(*property);
//						auto sampler = texture->sampler();
//						sampler->maxAnisotropy(anisotropy);
//					}
				}
			}
		}
	}
}
