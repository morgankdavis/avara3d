//
//  App.cc
//  geometry-primitives
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::geoprim;
using namespace std;

// [Private Constants]

const Log::Level                  APP_LOG_LEVEL {Log::Level::Debug};
const uvec2                       WINDOW_SIZE {1280, 768};
const bool                        FULLSCREEN {false};
const bool                        ENABLE_HIGH_DPI {true};
const RenderContext::Antialiasing ANTIALIASING {RenderContext::Antialiasing::Msaa4X};
const bool                        ENABLE_VSYNC {false};
const bool                        CAPTURE_CURSOR {false};
const float                       TIMESTEP {1.0 / 120.0};
const bool                        DARK {false};

// [Public Lifecycle Functions]

App::App(int argc, char* argv[]):
    Application(argc, argv, APP_LOG_LEVEL) {}

App::~App() = default;

// [Application Protected Member Functions]

std::unique_ptr<Scene> App::init() {
    try {
        _window = make_unique<Window>(RenderContext::RenderingApi::OpenGL, *util::fs::ExecutableName(),
                                      WINDOW_SIZE, FULLSCREEN, ENABLE_HIGH_DPI, ANTIALIASING);
        _window->vSyncEnabled(ENABLE_VSYNC);
        _window->cursorCaptured(CAPTURE_CURSOR);

        auto visualWorld = make_unique<VisualWorld>(*_window);
        visualWorld->background(Background {Color::Black()});

        auto scene = make_unique<Scene>(std::move(visualWorld), nullptr, Window::InputContext());
        scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

        auto ambientLight = make_shared<AmbientLight>(Color(0.1f));
        auto ambientLightNode = Node::LightNode(ambientLight);
        scene->rootNode()->addChild(ambientLightNode);

        auto pointLight = make_shared<PointLight>(Color::LightGray());
        auto pointLightNode = Node::LightNode(pointLight);
        pointLightNode->position({0, 0, 5});
        auto material = make_shared<Material>(monostate {}, monostate {}, monostate {}, Color::White());
        auto sphere = Sphere::Mesh(0.25f, 12, material);
        pointLightNode->mesh(sphere);
        auto pointLightPivotNode = Node::NamedNode("point light pivot");
        pointLightPivotNode->addChild(pointLightNode);
        scene->rootNode()->addChild(pointLightPivotNode);
        _pointLightPivotNode = pointLightPivotNode;

        {
            auto mesh = Box::Mesh(1.5f, 1.0f, 1.5f);
            auto node = make_shared<Node>();
            node->name("box");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->rotation({0.0f, 1.0f, 0.0f}, radians(-45.0f));
            node->position(vec3(1.67f, -2.5f, 0.0f));
        }

        {
            auto mesh = Capsule::Mesh(0.5f, 1.0f);
            auto node = make_shared<Node>();
            node->name("capsule");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->position(vec3(-5.0f, -2.5f, 0.0f));
        }

        {
            auto mesh = Cone::Mesh(1.0f, 2.0f);
            auto node = make_shared<Node>();
            mesh->name("cone");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            //node->position(vec3(-5.0f, 0.0f, 0.0f));
            node->position(vec3(-1.67f, -2.5f, 0.0f));
        }

        {
            auto mesh = Cylinder::Mesh(0.5f, 2.0f);
            auto node = make_shared<Node>();
            mesh->name("cylinder");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->position(vec3(5.0f, -2.5f, 0.0f));
        }

        {
            auto mesh = Plane::Mesh(10.0f, 10.0f);
            auto node = make_shared<Node>();
            mesh->name("plane");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->rotation({-1.0f, 0.0f, 0.0f}, radians(90.0f));
            node->position(vec3(0.0f, -5.0f, 0.0f));
        }

        {
            auto mesh = RoundedBox::Mesh(0.25f, 1, 1, 1);
            auto node = make_shared<Node>();
            mesh->name("rounded box");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->rotation({0.0f, 1.0f, 0.0f}, radians(70.0f));
            node->position(vec3(-3.0f, 2.5f, 0.0f));
        }

        {
            auto mesh = Sphere::Mesh(1.0f);
            auto node = make_shared<Node>();
            mesh->name("sphere");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->position(vec3(3.0f, 2.5f, 0.0f));
        }

        {
            auto mesh = Spring::Mesh(0.2f, 0.5f, 2.5f);
            auto node = make_shared<Node>();
            mesh->name("spring");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->position(vec3(0.0f, 3.5f, 0.0f));
            node->rotation({0.0f, 1.0f, 0.0f}, radians(-90.0f));
        }

        {
            auto mesh = Torus::Mesh(0.75f, 1.0f);
            auto node = make_shared<Node>();
            mesh->name("torus");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->position(vec3(4.25f, 0.0f, 0.0f));
        }

        {
            auto mesh = TorusKnot::Mesh(2, 3);
            auto node = make_shared<Node>();
            mesh->name("torus knot");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->rotation({0.0f, 1.5f, 0.0f}, radians(45.0f));
            node->position(vec3(0.0f, 0.0f, 0.0f));
        }

        {
            auto mesh = Tube::Mesh(0.5f, 0.75f, 2.0f);
            auto node = make_shared<Node>();
            mesh->name("tube");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->position(vec3(-4.25f, 0.0f, 0.0f));
        }

        {
            auto mesh = Wedge::Mesh(2.0f, 2.0f, 1.0f);
            auto node = make_shared<Node>();
            mesh->name("wedge");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->position(vec3(2.5f, 5.0f, 0.0f));
            node->rotation({0.0f, 1.0f, 0.0f}, radians(30.0f));
        }

        {
            auto mesh = Dome::Mesh(1.0f, math::radians(60.0), math::radians(90.0), 0, math::radians(180.0));
            auto node = make_shared<Node>();
            mesh->name("dome");
            node->mesh(mesh);
            scene->rootNode()->addChild(node);
            node->position(vec3(-2.5f, 5.0f, 0.0f));
            node->rotation({0.0f, 1.0f, 0.0f}, radians(-30.0f));
        }

        auto cameraConfig = _cameraController.config();
        cameraConfig.moveSpeed = math::max(scene->extent());
        _cameraController.config(cameraConfig);

        _window->center();
        _window->open();

        return scene;
    }
    catch (std::exception& e) {
        log::app::f()("Exception: {}", e.what());
        return nullptr;
    }
}

SimulationConfig App::simulationConfig() const {
    return {.timeStep = TIMESTEP};
}

bool App::shouldContinue(const Scene& scene) {
    return _window->isOpen();
}

void App::inputDidUpdate(Runner&                         runner,
                         Scene&                          scene,
                         InputContext&                   inputContext,
                         const InputContext::UpdateInfo& info) {

    Window* window = nullptr;
    if (scene.visualWorld()) {
        window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());
    }

    // get input

    auto input = static_cast<DesktopInputContext*>(&inputContext);

    using Key = DesktopInputContext::Key;

    if (input->keyPressed(Key::Slash)) {
        window->cursorCaptured(!(window->cursorCaptured()));
    }

    if (input->keyPressed(Key::Escape)) {
        window->close();
    }

    using DebugOptions = Scene::DebugOptions;

    if (input->keyPressed(Key::F)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowMeshWireframes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowMeshWireframes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowMeshWireframes));
        }
    }
    if (input->keyPressed(Key::B)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowMeshBounds)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowMeshBounds));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowMeshBounds));
        }
    }

    if (auto pov = scene.visualWorld()->pointOfView().lock(); pov && _window->cursorCaptured()) {
        _cameraController.update(*pov, *input, info.deltaTime);
    }
}

void App::sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    if (auto pivot = _pointLightPivotNode.lock()) {
        const float rotation = static_cast<float>(info.deltaTime) * radians(-30.0f);
        const auto  angles = pivot->eulerAngles();
        pivot->eulerAngles({0.0f, angles.y - rotation, 0.0f});
    }
}
