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



	_viewport->scene = _scene.get();





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
constexpr bool 					ORTHO_CAMERA =			false;
constexpr bool					DARK =					false;



void MainWindow::initA3D(A3DViewport& viewport) {

	//return;

	using utils::MeshNamed;

	try {
		initLog();
		logBuildInfo();

		auto visualWorld = make_unique<VisualWorld>(viewport);

//		MaterialProperty background = monostate{};
//		if (DARK) background = Color::Black();
//		else background = make_shared<Texture>(utils::CubeImageNamed("kloppenheim", "png"));
//		visualWorld->background(background);
//		visualWorld->willRender(bind(&MainWindow::willRenderCallback, this, _1, _2, _3));
//		visualWorld->didRender(bind(&MainWindow::didRenderCallback, this, _1, _2, _3));
//
//		auto physicalWorld = make_unique<PhysicalWorld>();
//		physicalWorld->timestep(PHYSICS_TIMESTEP);
//		physicalWorld->didSimulate(bind(&MainWindow::didSimulatePhysicsCallback, this, _1, _2, _3));
//
//		//auto scene = make_unique<Scene>(std::move(visualWorld), std::move(physicalWorld), std::move(inputManager));
//		_scene = make_unique<Scene>(std::move(visualWorld), std::move(physicalWorld), nullptr);
//		_scene->debugOptions(DebugOptions::ShowStatsOverlay);
//		_scene->update(bind(&MainWindow::updateCallback, this, _1, _2, _3));
//
//		auto ambientLight = make_shared<AmbientLight>(Color::DarkGray());
//		auto ambientLightNode = Node::LightNode(ambientLight);
//		_scene->rootNode()->addChild(ambientLightNode);
//
//
//
//
//
//		// ground plane
//
//		const float PLANE_LENGTH = 20.0;
//		const float PLANE_WIDTH = 20.0;
//		auto planeNode = make_shared<Node>("Ground plane node");
//		//planeNode->mesh(Mesh::Box(PLANE_LENGTH, PLANE_WIDTH, 0));
//		planeNode->mesh(Box::Mesh(PLANE_LENGTH, PLANE_WIDTH, 0));
//		auto gridImage = DARK ? utils::ImageNamed("grid10")->inverted() : utils::ImageNamed("grid10");
//		auto planeTexture = make_shared<Texture>(std::move(gridImage));
//		planeTexture->sampler()->wrapS(WrapMode::Repeat);
//		planeTexture->sampler()->wrapT(WrapMode::Repeat);
//		planeTexture->sampler()->maxAnisotropy(16);
//		planeTexture->sampler()->minificationFilter(FilterMode::LinearMipmapLinear);
//		planeTexture->sampler()->magnificationFilter(FilterMode::Linear);
//		shared_ptr<Material> planeMaterial = nullptr;
//		if (DARK) {
//			planeMaterial = make_shared<Material>(monostate{},
//												  monostate{},
//												  Color::White(),
//												  planeTexture);
//		}
//		else {
//			planeMaterial = make_shared<Material>(monostate{},
//												  planeTexture,
//												  monostate{});
//		}
////
//		planeMaterial->uvScale(PLANE_LENGTH/10.0f);
//		planeMaterial->doubleSided(false);
//		planeNode->mesh()->addMaterial(planeMaterial);
//		planeNode->rotation({1, 0, 0}, radians(3*90.0));
//		planeNode->position({planeNode->position().x, 0, planeNode->position().z});
//
//		_scene->rootNode()->addChild(planeNode);
//
//
//
//
//
//
//
//		auto pointLight = make_shared<PointLight>(Color::LightGray());
//		pointLight->constantAttenuation(1.0);
//		auto pointLightNode = Node::LightNode(pointLight);
//		pointLightNode->position({5, 5, 0});
//
//		auto material = make_shared<Material>(monostate{},
//											  monostate{},
//											  monostate{},
//											  Color::White());
//
//		auto sphere = Sphere::Mesh(0.1f, 12, material);
//
//		pointLightNode->mesh(sphere);
//
//		_scene->rootNode()->addChild(pointLightNode);







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
//		scene->inputManager(std::move(inputManager));
		_scene->debugOptions(DebugOptions::ShowStatsOverlay);
		_scene->update(bind(&MainWindow::updateCallback, this, _1, _2, _3));

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


