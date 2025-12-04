#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include "glm/glm.hpp"

#include "a3d/a3d.h"
#include "a3d/Utilities.h"

#include "QtViewport.h"
#include "QtInputManager.h"


using namespace a3d;
using namespace a3de;
using namespace glm;
using namespace std;
using namespace std::placeholders;


constexpr LogLevel				A3D_APP_LOG_LEVEL =		LogLevel::Debug;
constexpr uvec2					WINDOW_SIZE =			{1280, 768};
constexpr bool					FULLSCREEN =			false;
constexpr bool					ENABLE_HIGH_DPI =		true;
constexpr AntialiasingMode		MSAA_MODE =				AntialiasingMode::Msaa4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr float					MOUSE_SENSITIVITY =		0.5;
constexpr float					PHYSICS_TIMESTEP =		1.0/120.0;
constexpr bool 					ORTHO_CAMERA =			false;
constexpr bool					DARK =					false;


MainWindow::MainWindow(QWidget* parent):
		QMainWindow(parent),
		_ui(new Ui::MainWindow) {

	_ui->setupUi(this);
	statusBar()->hide();

	_viewport = new a3d::head::qt::QtViewport(RenderingApi::OpenGL, this);
	initScene(*_viewport);
	_viewport->scene(_scene.get());

	setCentralWidget(_viewport);
}

MainWindow::~MainWindow() {
	delete _ui;
}

void MainWindow::initScene(a3d::head::qt::QtViewport &viewport) {

	using utils::MeshNamed;

	try {
		initLog();
		logBuildInfo();

		auto inputManager = make_unique<a3d::head::qt::QtInputManager>(*_viewport);
		if (inputManager->errorMask() == DesktopInputManagerErrorMask::PermissionDenied) {
			A3D_APP_LOG_E(_log, "GLFWInputManager permission denied.");
			// on macOS 10.15 Catalina+, this is probably a permissions issue,
			// and the OS will alert the user.
			// just keep going and let the user decide what they want to do.
		}

		auto visualWorld = make_unique<VisualWorld>(viewport);

		visualWorld->fogStartDistance(500.0);
		visualWorld->fogEndDistance(5000.0);
		visualWorld->fogDensityExponent(1.0);
		visualWorld->fogColor(Color::LightGray());
		//visualWorld->usesDefaultLighting(true);
		visualWorld->willRender(bind(&MainWindow::willRenderCallback, this, _1, _2, _3));
		visualWorld->didRender(bind(&MainWindow::didRenderCallback, this, _1, _2, _3));
		visualWorld->background(make_shared<Texture>(std::move(utils::CubeImageNamed("nebula1_blue", "png"))));

		_scene = utils::SceneNamed("cat_island/cat_island", SceneImportOptions::ImportMeshes
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
		_pointLightNode = pointLightNode.get(); // <- how is this not crashing?
		auto material = make_shared<Material>();
		material->name("LIGHT material");
		material->emission(Color::White());
		auto geometry = Sphere::Mesh(1.5, 4, material);
		pointLightNode->mesh(geometry);
		_scene->rootNode()->addChild(pointLightNode);
	}
	catch (Exception& e)
	{
		A3D_APP_LOG_F(_log, "Exception: {}", e.what());
		//return -1;
	}
}

void MainWindow::initLog() {
	string executableName = *utils::ExecutableName();
	auto nativeSink = make_unique<StdOutLogSink>();
	auto fileSink = make_unique<FileLogSink>(*(utils::ExecutableDirectory())
											 / (executableName + string(".log")));
	auto sinks = unordered_set<unique_ptr<LogSink>>();
	sinks.insert(std::move(nativeSink));
	sinks.insert(std::move(fileSink));

	_log = make_unique<Log>(executableName, std::move(sinks));
	_log->level(A3D_APP_LOG_LEVEL);

	Log::MainLog().level(A3D_APP_LOG_LEVEL);
}

void MainWindow::logBuildInfo() {

	auto buildInfo = BuildInfo::Info();
	auto version = buildInfo.version();
	A3D_APP_LOG_I(_log, "A3D version: {}.{}.{}", version.major, version.minor, version.patch);
	A3D_APP_LOG_I(_log, "Build: {}", buildInfo.number());
	A3D_APP_LOG_I(_log, "Type: {}", buildInfo.type() == BuildInfo::Type::Debug ? "Debug" : "Release");
	A3D_APP_LOG_I(_log, "Origin: {}", buildInfo.origin() == BuildInfo::Origin::CI ? "CI" : "AdHoc");
}


void MainWindow::updateCallback(a3d::Scene& scene, double time, double deltaTime) {
	A3D_APP_LOG_T(_log, "");

//	static int invocations = 0;
//	if (invocations == 2) {
//		double time = utils::Time() - g_startTime;
//		A3D_APP_LOG_I(_log, "START TIME: {}", time);
//	}
//	++invocations;
//
//	A3D_APP_LOG_T(_log, "scene: {:p}, time: {}, deltaTime: {}", (void*)&scene, time, deltaTime);

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
		A3D_APP_LOG_I(_log, "TREE:\n{}", utils::StringFromTree(*(scene.rootNode())));
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
		utils::SaveSnapshot(*viewport);
	}

	if (keysPressed.count(Key::R)) {
		if (!viewport->recordingGIF()) {
			utils::StartGIFRecording(*viewport, {320, 240}, 8);
		}
		else {
			utils::StopGIFRecording(*viewport);
		}
	}

	if (keysPressed.count(Key::Slash)) {
		viewport->cursorCaptured(!(viewport->cursorCaptured()));
	}

	if (viewport->cursorCaptured()) {

		auto mouseButtonsPressed = im->mouseButtonsPressed();
		if (mouseButtonsPressed.count(MouseButton::One)) {
			A3D_APP_LOG_D(_log, "one");
		}
		if (mouseButtonsPressed.count(MouseButton::Two)) {
			A3D_APP_LOG_D(_log, "two");
		}
		if (mouseButtonsPressed.count(MouseButton::Three)) {
			A3D_APP_LOG_D(_log, "three");
		}

		auto scrollWheelDelta = im->mouseScrollWheelDelta();
		if (fabs(scrollWheelDelta.x) > .0001) {
			A3D_APP_LOG_D(_log, "x: {}", scrollWheelDelta.x);
		}
		else if (fabs(scrollWheelDelta.y) > .0001) {
			A3D_APP_LOG_D(_log, "y: {}", scrollWheelDelta.y);
		}

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

			float deltaRotX = atan(MOUSE_SPEED * mousePositionDelta.x);
			float deltaRotY = atan(MOUSE_SPEED * mousePositionDelta.y);

			//A3D_LOG_I("delta: ({}, {})", mousePositionDelta.x, mousePositionDelta.y);

			vec3 angles = pov->eulerAngles();
			pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));

			// move

//			auto keysDown = im->keysDown();

			static float MOVE_SPEED = 0;
			if (!MOVE_SPEED) MOVE_SPEED = utils::Max(scene.rootNode()->extent());

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

		float x = sin(angle) * radiusX;
		float y = cos(angle) * radiusY;

		_pointLightNode->position(center + vec3(x, y, -x));
	}
}

void MainWindow::willRenderCallback(a3d::VisualWorld& world, double time, double deltaTime) {
	A3D_APP_LOG_T(_log, "");
}

void MainWindow::didRenderCallback(a3d::VisualWorld& world, double time, double deltaTime) {
	A3D_APP_LOG_T(_log, "");
}

void MainWindow::didSimulatePhysicsCallback(a3d::PhysicalWorld& world, double time, double deltaTime) {
	A3D_APP_LOG_T(_log, "");
}
