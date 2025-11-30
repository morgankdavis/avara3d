#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "a3dviewport.h"

#include "glm/glm.hpp"

#include "a3d/a3d.h"
#include "a3d/Utilities.h"


using namespace a3d;
using namespace a3de;


MainWindow::MainWindow(QWidget* parent):
		QMainWindow(parent),
		ui(new Ui::MainWindow) {

	ui->setupUi(this);

	_viewport = new A3DViewport(RenderingApi::OpenGL, this);

	initA3D(*_viewport);

	setCentralWidget(_viewport);
}

MainWindow::~MainWindow()
{
	delete ui;
}






//using namespace a3d;
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
constexpr bool					DARK =					false;



void MainWindow::initA3D(A3DViewport& viewport) {

	using utils::MeshNamed;

	try {
		initLog();
		logBuildInfo();

//		auto window = make_unique<Window>(RenderingApi::OpenGL,
//										  *utils::ExecutableName(),
//										  WINDOW_SIZE,
//										  FULLSCREEN,
//										  ENABLE_HIGH_DPI,
//										  MSAA_MODE);
//		window->vSyncEnabled(ENABLE_VSYNC);
//		window->cursorCaptured(CAPTURE_CURSOR);

		viewport.vSyncEnabled(ENABLE_VSYNC);
//		viewport.cursorCaptured(CAPTURE_CURSOR);


		//auto inputManager = make_unique<WindowInputManager>(window.get());
//		auto inputManager = make_unique<WindowInputManager>(viewport);
//		if (inputManager->errorMask() == WindowInputManagerErrorMask::PermissionDenied) {
//			A3D_APP_LOG_E(_logger, "WindowInputManager permission denied.");
//			// on macOS 10.15 Catalina+, this is probably a permissions issue,
//			// and the OS will alert the user.
//			// just keep going and let the user decide what they want to do.
//		}

//		auto visualWorld = make_unique<VisualWorld>(*window);
		auto visualWorld = make_unique<VisualWorld>(viewport);

		MaterialProperty background = monostate{};
		if (DARK) background = Color::Black();
		else background = make_shared<Texture>(utils::CubeImageNamed("kloppenheim", "png"));
		visualWorld->background(background);
		visualWorld->willRender(bind(&MainWindow::willRenderCallback, this, _1, _2, _3));
		visualWorld->didRender(bind(&MainWindow::didRenderCallback, this, _1, _2, _3));

		auto physicalWorld = make_unique<PhysicalWorld>();
		physicalWorld->timestep(PHYSICS_TIMESTEP);
		physicalWorld->didSimulate(bind(&MainWindow::didSimulatePhysicsCallback, this, _1, _2, _3));

		//auto scene = make_unique<Scene>(std::move(visualWorld), std::move(physicalWorld), std::move(inputManager));
		auto scene = make_unique<Scene>(std::move(visualWorld), std::move(physicalWorld), nullptr);
		scene->debugOptions(DebugOptions::ShowStatsOverlay);
		scene->update(bind(&MainWindow::updateCallback, this, _1, _2, _3));

		auto ambientLight = make_shared<AmbientLight>(Color::DarkGray());
		auto ambientLightNode = Node::LightNode(ambientLight);
		scene->rootNode()->addChild(ambientLightNode);





		// ground plane

		const float PLANE_LENGTH = 20.0;
		const float PLANE_WIDTH = 20.0;
		auto planeNode = make_shared<Node>("Ground plane node");
		//planeNode->mesh(Mesh::Box(PLANE_LENGTH, PLANE_WIDTH, 0));
		planeNode->mesh(Box::Mesh(PLANE_LENGTH, PLANE_WIDTH, 0));
		auto gridImage = DARK ? utils::ImageNamed("grid10")->inverted() : utils::ImageNamed("grid10");
		auto planeTexture = make_shared<Texture>(std::move(gridImage));
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

		planeMaterial->uvScale(PLANE_LENGTH/10.0f);
		planeMaterial->doubleSided(false);
		planeNode->mesh()->addMaterial(planeMaterial);
		planeNode->rotation({1, 0, 0}, radians(3*90.0));
		planeNode->position({planeNode->position().x, 0, planeNode->position().z});

		scene->rootNode()->addChild(planeNode);







		auto pointLight = make_shared<PointLight>(Color::LightGray());
		pointLight->constantAttenuation(1.0);
		auto pointLightNode = Node::LightNode(pointLight);
		pointLightNode->position({5, 5, 0});

		auto material = make_shared<Material>(monostate{},
											  monostate{},
											  monostate{},
											  Color::White());

		auto sphere = Sphere::Mesh(0.1f, 12, material);

		pointLightNode->mesh(sphere);

		scene->rootNode()->addChild(pointLightNode);


		scene->run();



	}
	catch (Exception& e)
	{
		A3D_APP_LOG_F(_logger, "Exception: {}", e.what());
		//return -1;
	}
}




void MainWindow::initLog() {
	string executableName = *utils::ExecutableName();
	auto nativeSink = make_unique<StdOutLoggerSink>();
	auto fileSink = make_unique<FileLoggerSink>(*(utils::ExecutableDirectory())
												/ (executableName + string(".log")));
	auto sinks = unordered_set<unique_ptr<LoggerSink>>();
	sinks.insert(std::move(nativeSink));
	sinks.insert(std::move(fileSink));

	_logger = make_unique<Logger>(executableName, std::move(sinks));
	_logger->level(A3D_APP_LOG_LEVEL);

	Logger::MainLogger().level(A3D_APP_LOG_LEVEL);
}

void MainWindow::logBuildInfo() {

	auto buildInfo = BuildInfo::Info();
	auto version = buildInfo.version();
	A3D_APP_LOG_I(_logger, "A3D version: {}.{}.{}", version.major, version.minor, version.patch);
	A3D_APP_LOG_I(_logger, "Build: {}", buildInfo.number());
	A3D_APP_LOG_I(_logger, "Type: {}", buildInfo.type() == BuildInfo::Type::Debug ? "Debug" : "Release");
	A3D_APP_LOG_I(_logger, "Origin: {}", buildInfo.origin() == BuildInfo::Origin::CI ? "CI" : "AdHoc");
}


void MainWindow::updateCallback(a3d::Scene& scene, double time, double deltaTime) {
	A3D_APP_LOG_T(_logger, "");
}

void MainWindow::willRenderCallback(a3d::VisualWorld& world, double time, double deltaTime) {
	A3D_APP_LOG_T(_logger, "");
}

void MainWindow::didRenderCallback(a3d::VisualWorld& world, double time, double deltaTime) {
	A3D_APP_LOG_T(_logger, "");
}

void MainWindow::didSimulatePhysicsCallback(a3d::PhysicalWorld& world, double time, double deltaTime) {
	A3D_APP_LOG_T(_logger, "");
}


