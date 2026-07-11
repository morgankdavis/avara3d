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
		_ui(new Ui::MainWindow) {

	_ui->setupUi(this);
	statusBar()->hide();

	resize(WINDOW_SIZE.x, WINDOW_SIZE.y);

	_viewport = new head::qt::QtViewport(RenderContext::RenderingApi::OpenGL,
											  ANTIALIAS_MODE,
											  this);
	initScene(*_viewport);

	setCentralWidget(_viewport);
}

MainWindow::~MainWindow() {
	_runner.reset();
	delete _ui;
}

void MainWindow::initScene(head::qt::QtViewport &viewport) {

	using util::filesystem::MeshNamed;

	try {
		initLog();
		logBuildInfo();

		auto inputManager = make_unique<head::qt::QtInputManager>(*_viewport);

		auto visualWorld = make_unique<VisualWorld>(viewport);
		auto backgroundColor = make_shared<Color>(u8vec3{109, 136, 164});
		visualWorld->background(backgroundColor);
		visualWorld->willRenderCallback(bind(&MainWindow::willRenderCallback, this, _1, _2, _3));
		visualWorld->didRenderCallback(bind(&MainWindow::didRenderCallback, this, _1, _2, _3));

		auto scene = make_unique<Scene>();
		scene->visualWorld(std::move(visualWorld));
		scene->inputManager(std::move(inputManager));
		scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);
		scene->updateCallback(bind(&MainWindow::updateCallback, this, _1, _2, _3));

		viewport.cursorCaptured(CAPTURE_CURSOR);

		_runner = std::make_unique<Runner>(std::move(scene));
		_runner->start();

		_viewport->runner(_runner.get());
	}
	catch (std::exception& e)
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
	log::app::i()("A3D version: {}", BuildInfo::VersionString(buildInfo.version()));
	log::app::i()("Build: {}", buildInfo.number());
	log::app::i()("Type: {}", BuildInfo::TypeString(buildInfo.type()));
	log::app::i()("Origin: {}", BuildInfo::OriginString(buildInfo.origin()));
}

void MainWindow::updateCallback(Scene &scene, double time, double deltaTime) {
	//log::app::t();
}

void MainWindow::willRenderCallback(VisualWorld &world, double time, double deltaTime) {
	//log::app::t();
}

void MainWindow::didRenderCallback(VisualWorld &world, double time, double deltaTime) {
	//log::app::t();
}

void MainWindow::didSimulatePhysicsCallback(PhysicalWorld &world, double time, double deltaTime) {
	//log::app::t();
}
