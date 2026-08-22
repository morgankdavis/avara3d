//
//  MainWindow.cpp
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include "a3d/a3d.h"

#include "QtViewport.h"
#include "QtInputContext.h"

using namespace a3d;
using namespace a3de;
using namespace a3d::math;
using namespace std;
using namespace std::placeholders;

const log::Level                  APP_LOG_LEVEL {log::Level::Debug};
const uvec2                       WINDOW_SIZE {1280, 768};
const RenderContext::Antialiasing ANTIALIASING {RenderContext::Antialiasing::Msaa4X};
const bool                        CAPTURE_CURSOR {false};

MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _viewport(new qt::QtViewport(ANTIALIASING, this)),
    _scene {},
    _runner {} {

    _ui->setupUi(this);
    statusBar()->hide();

    resize(WINDOW_SIZE.x, WINDOW_SIZE.y);

    setCentralWidget(_viewport);

    connect(_viewport, &qt::QtViewport::initialized, this, &MainWindow::initA3D);

    connect(_viewport, &qt::QtViewport::renderFrame, this, &MainWindow::updateA3D);
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
        visualWorld->background(Background {Color(u8vec3 {109, 136, 164})});
        visualWorld->defaultLightingEnabled(true);

        _scene = make_unique<Scene>(std::move(visualWorld), nullptr, qt::QtViewport::InputContext());
        _scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

        _viewport->cursorCaptured(CAPTURE_CURSOR);

        _bananaNode = Node::MeshNode(util::fs::MeshAt("banana_lod/banana_lod.gltf"));
        auto rx = math::quaternion({1.0f, 0.0f, 0.0f}, radians(90.0f));
        auto ry = math::quaternion({0.0f, 1.0f, 0.0f}, radians(90.0f));
        _bananaNode->orientation(rx * ry);
        _scene->rootNode()->addChild(_bananaNode);

        _runner = make_unique<Runner>(*_scene);
        _runner->updateCallback(bind(&MainWindow::hostUpdate, this, _1, _2));
        _runner->start();
    }
    catch (std::exception& e) {
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

void MainWindow::initLog(log::Level level) {

    log::MainLog().level(level);

    string executableName = *util::fs::ExecutableName();

    auto nativeSink = make_unique<log::StdOutLogSink>();
    auto fileSink =
        make_unique<log::FileLogSink>(*(util::fs::ExecutableDirectory()) / (executableName + string(".log")));
    auto sinks = vector<unique_ptr<log::LogSink>>();
    sinks.push_back(std::move(nativeSink));
    sinks.push_back(std::move(fileSink));

    log::AppLog(make_unique<log::Log>(executableName, std::move(sinks), level));

    const auto& buildInfo = BuildInfo::Info();
    log::app::i()("A3D version: {}", BuildInfo::VersionString(buildInfo.version()));
    log::app::i()("Build: {}", buildInfo.number());
    log::app::i()("Type: {}", BuildInfo::TypeString(buildInfo.type()));
    log::app::i()("Origin: {}", BuildInfo::OriginString(buildInfo.origin()));
}

/// Runner Callbacks ///

void MainWindow::hostUpdate(Runner& runner, const Runner::UpdateInfo& info) {

    auto& inputContext = static_cast<DesktopInputContext&>(*runner.scene().inputContext());
    using Key = DesktopInputContext::Key;

    if (inputContext.keysPressed().count(Key::Escape)) {
        QCoreApplication::quit();
    }

    //log::app::t();

    if (_bananaNode) {
        // rotate the banana
        auto rotationDeg = info.deltaTime * radians(-30.0); // 10deg/sec

        auto rotY = math::quaternion({0.0f, 1.0f, 0.0f}, rotationDeg);
        _bananaNode->orientation(rotY * _bananaNode->orientation());
    }
}
