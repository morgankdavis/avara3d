//
//  MainWindow.cpp
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include "a3d/a3d.h"
#include "a3d/util/filesystem.h"
#include "a3d/util/snapshot.h"
#include "a3d/util/string.h"

#include "QtViewport.h"
#include "QtInputManager.h"

using namespace a3d;
using namespace a3de;
using namespace a3d::math;
using namespace std;
using namespace std::placeholders;

const LogLevel				APP_LOG_LEVEL			{LogLevel::Debug};
const uvec2					WINDOW_SIZE				{1280, 768};
const AntialiasingMode		AA_MODE					{AntialiasingMode::Msaa4X};
const bool					CAPTURE_CURSOR			{false};
const float					MOUSE_SENSITIVITY		{0.5};

MainWindow::MainWindow(QWidget* parent):
		QMainWindow(parent),
		_ui(new Ui::MainWindow) {

	_ui->setupUi(this);
	statusBar()->hide();

	resize(WINDOW_SIZE.x, WINDOW_SIZE.y);

	_viewport = new a3d::head::qt::QtViewport(RenderingApi::OpenGL,
											  AA_MODE,
											  this);
	initScene(*_viewport);
	_viewport->scene(_scene.get());

	setCentralWidget(_viewport);
}

MainWindow::~MainWindow() {
	delete _ui;
}

void MainWindow::initScene(a3d::head::qt::QtViewport &viewport) {

	using util::filesystem::MeshNamed;

	try {
		initLog();
		logBuildInfo();

		auto inputManager = make_unique<a3d::head::qt::QtInputManager>(*_viewport);

		auto visualWorld = make_unique<VisualWorld>(viewport);

		visualWorld->fogStartDistance(500.0);
		visualWorld->fogEndDistance(5000.0);
		visualWorld->fogDensityExponent(1.0);
		visualWorld->fogColor(Color::LightGray());
		//visualWorld->usesDefaultLighting(true);
		visualWorld->willRenderCallback(bind(&MainWindow::willRenderCallback, this, _1, _2, _3));
		visualWorld->didRenderCallback(bind(&MainWindow::didRenderCallback, this, _1, _2, _3));
		visualWorld->background(make_shared<Texture>(std::move(util::filesystem::CubeImageNamed("nebula1_blue", "png"))));

		_scene = util::filesystem::SceneNamed("cat_island/cat_island", SceneImportOptions::ImportMeshes
																| SceneImportOptions::ImportMaterials
																| SceneImportOptions::ImportCameras);

		_scene->visualWorld(std::move(visualWorld));
		_scene->inputManager(std::move(inputManager));
		_scene->debugOptions(DebugOptions::ShowStatsOverlay);
		_scene->updateCallback(bind(&MainWindow::updateCallback, this, _1, _2, _3));

		auto ambientLight = make_shared<AmbientLight>(make_shared<Color>(0.1f));
		ambientLight->name("ambient");
		auto ambientLightNode = Node::LightNode(ambientLight);
		_scene->rootNode()->addChild(ambientLightNode);

		auto pointLight = make_shared<PointLight>(Color::White());
		pointLight->name("point");
		pointLight->quadraticAttenuation(0.002);
		auto pointLightNode = Node::LightNode(pointLight);
		_pointLightNode = pointLightNode; // <- how is this not crashing?
		auto material = make_shared<Material>();
		material->name("LIGHT material");
		material->emission(Color::White());
		auto geometry = Sphere::Mesh(1.5, 4, material);
		pointLightNode->mesh(geometry);
		_scene->rootNode()->addChild(pointLightNode);

//		if (FULLSCREEN) {
//			showFullScreen(); // blows up ?
//		}

		viewport.cursorCaptured(CAPTURE_CURSOR);
	}
	catch (Exception& e)
	{
		log::app::f()("Exception: {}", e.what());
		//return -1;
	}
}

void MainWindow::initLog() {

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

void MainWindow::logBuildInfo() {

	auto buildInfo = BuildInfo::Info();
	auto version = buildInfo.version();
	log::app::i()("A3D version: {}.{}.{}", version.major, version.minor, version.patch);
	log::app::i()("Build: {}", buildInfo.number());
	log::app::i()("Type: {}", buildInfo.type() == BuildInfo::Type::Debug ? "Debug" : "Release");
	log::app::i()("Origin: {}", buildInfo.origin() == BuildInfo::Origin::CI ? "CI" : "AdHoc");
}


void MainWindow::updateCallback(a3d::Scene& scene, double time, double deltaTime) {
	log::app::t();

//	static int invocations = 0;
//	if (invocations == 2) {
//		double time = utils::Time() - g_startTime;
//		log::app::i()("START TIME: {}", time);
//	}
//	++invocations;
//
//	log::app::t()("scene: {:p}, time: {}, deltaTime: {}", (void*)&scene, time, deltaTime);

	//auto window = dynamic_cast<GlfwWindow*>(scene.visualWorld()->renderContext());

	// get input




	auto viewport = dynamic_cast<a3d::head::qt::QtViewport*>(scene.visualWorld()->renderContext());




	auto im = static_cast<a3d::head::qt::QtInputManager*>(scene.inputManager());
	auto keysPressed = im->keysPressed();
	auto keysDown = im->keysDown();

	if (keysPressed.count(Key::Escape)) {
		//window->close();
		QCoreApplication::quit();
	}

	if (keysPressed.count(Key::T)) {
		log::app::i()("TREE:\n{}", util::string::TreeString(*(scene.rootNode())));
	}

//	if 		(keysPressed.count(Key::One))	SetAllFilterModes(FilterMode::Nearest, scene);
//	else if (keysPressed.count(Key::Two))	SetAllFilterModes(FilterMode::Linear, scene);
//	else if (keysPressed.count(Key::Three))	SetAllFilterModes(FilterMode::NearestMipmapNearest, scene);
//	else if (keysPressed.count(Key::Four))	SetAllFilterModes(FilterMode::NearestMipmapLinear, scene);
//	else if (keysPressed.count(Key::Five))	SetAllFilterModes(FilterMode::LinearMipmapNearest, scene);
//	else if (keysPressed.count(Key::Six))	SetAllFilterModes(FilterMode::LinearMipmapLinear, scene);
//
//	if 		(keysPressed.count(Key::LeftBracket))	SetAllMaxAnisotropy(1, scene);
//	else if (keysPressed.count(Key::RightBracket))	SetAllMaxAnisotropy(16, scene);

//	if (auto attenuatedLight = dynamic_cast<AttenuatedLight*>(g_pointLightNode->light().get())) {
//		if (keysPressed.count(Key::F1)) attenuatedLight->constantAttenuation(1.0 - 0.0005);
//		else if (keysPressed.count(Key::F2)) attenuatedLight->constantAttenuation(1.0 - 0.00015);
//		else if (keysPressed.count(Key::F3)) attenuatedLight->constantAttenuation(1.0 - 0.00005);
//	}

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

//	if (keysPressed.count(Key::V)) {
//		window->vSyncEnabled(!(window->vSyncEnabled()));
//	}

	if (keysPressed.count(Key::Backslash)) {
		util::snapshot::SaveSnapshot(*viewport);
	}

	if (keysPressed.count(Key::R)) {
		if (!viewport->recordingGIF()) {
			util::snapshot::StartGIFRecording(*viewport, {320, 240}, 8);
		}
		else {
			util::snapshot::StopGIFRecording(*viewport);
		}
	}

	if (keysPressed.count(Key::Slash)) {
		viewport->cursorCaptured(!(viewport->cursorCaptured()));
	}

	if (viewport->cursorCaptured()) {

//		auto mouseButtonsPressed = im->mouseButtonsPressed();
//		if (mouseButtonsPressed.count(MouseButton::One)) {
//			log::app::d()("one");
//		}
//		if (mouseButtonsPressed.count(MouseButton::Two)) {
//			log::app::d()("two");
//		}
//		if (mouseButtonsPressed.count(MouseButton::Three)) {
//			log::app::d()("three");
//		}
//
//		auto scrollWheelDelta = im->mouseScrollWheelDelta();
//		if (fabs(scrollWheelDelta.x) > .0001) {
//			log::app::d()("x: {}", scrollWheelDelta.x);
//		}
//		else if (fabs(scrollWheelDelta.y) > .0001) {
//			log::app::d()("y: {}", scrollWheelDelta.y);
//		}

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

			//log::i()("delta: ({}, {})", mousePositionDelta.x, mousePositionDelta.y);

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

	if (_pointLightNode) {

		auto center = vec3(0, 30, 0);

		static auto extent = scene.rootNode()->extent();
		static float radius = std::max(std::max(extent.x, extent.y), extent.z) * .46;
		static float radiusX = radius;
		static float radiusY = radius;

		static float rotationSpeed = radians(30.0); // deg/secs
		static float angle = 0;
		angle += rotationSpeed * deltaTime;

		float x = math::sin(angle) * radiusX;
		float y = math::cos(angle) * radiusY;

		_pointLightNode->position(center + vec3(x, y, -x));
	}
}

void MainWindow::willRenderCallback(a3d::VisualWorld& world, double time, double deltaTime) {
	log::app::t();
}

void MainWindow::didRenderCallback(a3d::VisualWorld& world, double time, double deltaTime) {
	log::app::t();
}

void MainWindow::didSimulatePhysicsCallback(a3d::PhysicalWorld& world, double time, double deltaTime) {
	log::app::t();
}
