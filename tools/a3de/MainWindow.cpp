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
#include "a3d/util/Filesystem.h"

#include "QtViewport.h"
#include "QtInputManager.h"

using namespace a3d;
using namespace a3de;
using namespace a3d::math;
using namespace std;
using namespace std::placeholders;

const Log::Level						APP_LOG_LEVEL		{Log::Level::Debug};
const uvec2								WINDOW_SIZE			{1280, 768};
const RenderContext::AntialiasingMode	ANTIALIAS_MODE		{RenderContext::AntialiasingMode::Msaa4X};
const bool								CAPTURE_CURSOR		{false};
const float								MOUSE_SENSITIVITY	{0.5};

MainWindow::MainWindow(QWidget* parent):
		QMainWindow(parent),
		_ui(new Ui::MainWindow),
		_viewport(new qt::QtViewport(
			RenderContext::RenderingApi::OpenGL,
			ANTIALIAS_MODE,
			this)),
		_scene{},
		_runner{} {

	_ui->setupUi(this);
	statusBar()->hide();

	resize(WINDOW_SIZE.x, WINDOW_SIZE.y);

	setCentralWidget(_viewport);

	connect(
		_viewport,
		&qt::QtViewport::initialized,
		this,
		&MainWindow::initA3D
	);

	connect(
		_viewport,
		&qt::QtViewport::renderFrame,
		this,
		&MainWindow::updateA3D
	);
}

MainWindow::~MainWindow() {

	if (_runner) {
		_runner->stop();
		_runner.reset();
	}

	// Scene dies before QMainWindow destroys QtViewport
	_scene.reset();

	delete _ui;
}

void MainWindow::initA3D() {

	if (_scene) {
		return;
	}

	try {
		initLog(APP_LOG_LEVEL);

		auto visualWorld = make_unique<VisualWorld>(*_viewport);
		auto backgroundColor = make_shared<Color>(u8vec3{109, 136, 164});
		visualWorld->background(backgroundColor);
		visualWorld->willRenderCallback(bind(&MainWindow::willRenderCallback, this, _1, _2, _3));
		visualWorld->didRenderCallback(bind(&MainWindow::didRenderCallback, this, _1, _2, _3));

		_scene = make_unique<Scene>(std::move(visualWorld),
									nullptr,
									qt::QtViewport::InputManager());
		_scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);
		_scene->updateCallback(bind(&MainWindow::updateCallback, this, _1, _2, _3));

		_viewport->cursorCaptured(CAPTURE_CURSOR);

		_bananaNode = Node::MeshNode(util::filesystem::MeshNamed("banana_lod/banana_lod"));
		auto rot90X = math::quaternion({1.0f, 0.0f, 0.0f}, radians(90.0f));
		auto rot90Y = math::quaternion({0.0f, 1.0f, 0.0f}, radians(90.0f));
		_bananaNode->orientation(rot90X * rot90Y);
		_scene->rootNode()->addChild(_bananaNode);

		_runner = make_unique<Runner>(*_scene);
		_runner->start();
	}
	catch (std::exception& e)
	{
		log::app::f()("Exception: {}", e.what());
		//return -1;
	}
}

void MainWindow::updateA3D() {

	if (!_runner) {
		return;
	}

	if (_runner->update()) {
		_viewport->update();
	}
	else {
		_runner.reset();
		_scene.reset();
	}
}

void MainWindow::initLog(Log::Level level) {

	Log::MainLog().level(level);

	string executableName = *util::filesystem::ExecutableName();

	auto nativeSink = make_unique<StdOutLogSink>();
	auto fileSink = make_unique<FileLogSink>(*(util::filesystem::ExecutableDirectory())
											 / (executableName + string(".log")));
	auto sinks = vector<unique_ptr<LogSink>>();
	sinks.push_back(std::move(nativeSink));
	sinks.push_back(std::move(fileSink));

	Log::AppLog(make_unique<Log>(
		executableName,
		std::move(sinks),
		level));

	const auto& buildInfo = BuildInfo::Info();
	log::app::i()("A3D version: {}", BuildInfo::VersionString(buildInfo.version()));
	log::app::i()("Build: {}", buildInfo.number());
	log::app::i()("Type: {}", BuildInfo::TypeString(buildInfo.type()));
	log::app::i()("Origin: {}", BuildInfo::OriginString(buildInfo.origin()));
}

void MainWindow::updateCallback(Scene &scene, double time, double deltaTime) {
	//log::app::t();

	auto im = static_cast<DesktopInputManager*>(scene.inputManager());
	using Key = DesktopInputManager::Key;
	using MouseButton = DesktopInputManager::MouseButton;

	auto keysPressed = im->keysPressed();
	if (keysPressed.count(Key::Escape)) {
		QCoreApplication::quit();
	}

	if (_bananaNode) {
		// rotate the banana
		auto rotationDeg = deltaTime * radians(-30.0); // 10deg/sec

		auto rotY = math::quaternion({0.0f, 1.0f, 0.0f}, rotationDeg);
		_bananaNode->orientation(rotY * _bananaNode->orientation());
	}
}

void MainWindow::willRenderCallback(VisualWorld &world, double time, double deltaTime) {
	//log::app::t();
}

void MainWindow::didRenderCallback(VisualWorld &world, double time, double deltaTime) {
	//log::app::t();
}

void MainWindow::didSimulatePhysicsCallback(PhysicsWorld &world, double time, double deltaTime) {
	//log::app::t();
}
