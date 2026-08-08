//
//  App.cc
//  000-sandbox
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include "a3d/a3d.h"
#include "a3d/extension/ALFImporter.h" // temporary

using namespace a3d;
using namespace a3d::math;
using namespace test::sandbox;
using namespace std;

/// Private Constants ///

const Log::Level                      APP_LOG_LEVEL {Log::Level::Debug};
const uvec2                           WINDOW_SIZE {1280, 768};
const bool                            FULLSCREEN {false};
const bool                            ENABLE_HIGH_DPI {true};
const RenderContext::AntialiasingMode ANTIALIAS_MODE {RenderContext::AntialiasingMode::Msaa4X};
const bool                            ENABLE_VSYNC {false};
const bool                            CAPTURE_CURSOR {false};
const float                           MOUSE_SENSITIVITY {0.5};
const double                          TIMESTEP {1.0 / 120.0};
const bool                            DARK {false};

/// Public Lifecycle Functions ///

App::App(int argc, char* argv[]):
    Application(argc, argv, APP_LOG_LEVEL) {}

App::~App() = default;

/// Application Protected Member Functions ///

std::unique_ptr<Scene> App::init() {
    try {
        _window = make_unique<Window>(RenderContext::RenderingApi::OpenGL, *util::fs::ExecutableName(),
                                      WINDOW_SIZE, FULLSCREEN, ENABLE_HIGH_DPI, ANTIALIAS_MODE);
        _window->vSyncEnabled(ENABLE_VSYNC);
        _window->cursorCaptured(CAPTURE_CURSOR);

        auto visualWorld = make_unique<VisualWorld>(*_window);

        auto physicsWorld = make_unique<PhysicsWorld>();

        auto mapPath = util::fs::AuxiliaryFilePath("Icebox", "alf");
        auto alfImporter = ext::ALFImporter(*mapPath);
        auto scene = alfImporter.scene(*visualWorld);

//		else background = make_shared<Texture>(util::filesystem::CubeImageNamed("kloppenheim", "png"));
//		visualWorld->background(background);

        scene->visualWorld(std::move(visualWorld));
        scene->physicsWorld(std::move(physicsWorld));
        scene->inputContext(Window::InputContext());

        scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

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

    auto& input = static_cast<DesktopInputContext&>(inputContext);

    auto mousePositionDelta = input.mousePositionDelta();

    using Key = DesktopInputContext::Key;
    using MouseButton = DesktopInputContext::MouseButton;

    if (input.keyPressed(Key::Escape)) {
        window->close();
    }

    if (input.keyPressed(Key::T)) {
        log::app::i()("TREE:\n{}", util::string::TreeString(*(scene.rootNode())));
    }

    if (input.keyPressed(Key::P)) {
        if (auto pov = scene.visualWorld()->pointOfView().lock()) {
            auto world = pov->worldPosition() + pov->worldForward() * 10.0f;
            auto screen = scene.visualWorld()->projectPoint(world);
            auto roundTrip = scene.visualWorld()->unprojectPoint(screen);

            log::app::i()("world: {}, {}, {}", world.x, world.y, world.z);
            log::app::i()("screen: {}, {}, {}", screen.x, screen.y, screen.z);
            log::app::i()("roundTrip: {}, {}, {}", roundTrip.x, roundTrip.y, roundTrip.z);

            auto visualWorld = scene.visualWorld();
            auto viewportSize = visualWorld->renderContext()->viewportLogicalSize();

            const float centerX = float(viewportSize.x) * 0.5f;
            const float centerY = float(viewportSize.y) * 0.5f;

            auto nearPoint = visualWorld->unprojectPoint({centerX, centerY, 0.0f});
            auto farPoint = visualWorld->unprojectPoint({centerX, centerY, 1.0f});
            auto rayDirection = normalize(farPoint - nearPoint);

            if (auto pov = visualWorld->pointOfView().lock()) {
                auto cameraForward = pov->worldForward();
                log::app::i()("ray: {}, {}, {}", rayDirection.x, rayDirection.y, rayDirection.z);
                log::app::i()("forward: {}, {}, {}", cameraForward.x, cameraForward.y, cameraForward.z);
                log::app::i()("dot: {}", dot(rayDirection, cameraForward));
            }
        }
    }

    if (input.keyPressed(Key::One)) {

        //auto cameraNodes = vector<Node*>();
        auto cameraNodes = vector<std::shared_ptr<Node>>();
        for (auto& node : scene.rootNode()->children(true)) {
            auto camera = node->camera();
            if (camera) {
                cameraNodes.push_back(node);
            }
        }

        scene.visualWorld()->pointOfView(cameraNodes[0]);
    }
    if (input.keyPressed(Key::Two)) {

        auto cameraNodes = vector<std::shared_ptr<Node>>();
        for (auto& node : scene.rootNode()->children(true)) {
            auto camera = node->camera();
            if (camera) {
                cameraNodes.push_back(node);
            }
        }

        scene.visualWorld()->pointOfView(cameraNodes[1]);
    }

    if (input.keyPressed(Key::Three)) {

        auto cameraNodes = vector<std::shared_ptr<Node>>();
        for (auto& node : scene.rootNode()->children(true)) {
            auto camera = node->camera();
            if (camera) {
                cameraNodes.push_back(node);
            }
        }

        scene.visualWorld()->pointOfView(cameraNodes[2]);
    }

    using DebugOptions = Scene::DebugOptions;

    if (input.keyPressed(Key::F)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowWireframes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowWireframes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowWireframes));
        }
    }
    if (input.keyPressed(Key::B)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowBoundingBoxes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
        }
    }
    if (input.keyPressed(Key::I)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowStatsOverlay)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
        }
    }
    if (input.keyPressed(Key::P)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsBoundingBoxes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
                                                     DebugOptions::ShowPhysicsBoundingBoxes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(),
                                                  DebugOptions::ShowPhysicsBoundingBoxes));
        }
    }
    if (input.keyPressed(Key::G)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
                                                     DebugOptions::ShowPhysicsWireframes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes));
        }
    }
    if (input.keyPressed(Key::C)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsContactPoints)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
                                                     DebugOptions::ShowPhysicsContactPoints));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(),
                                                  DebugOptions::ShowPhysicsContactPoints));
        }
    }
    if (input.keyPressed(Key::N)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsNormals)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowPhysicsNormals));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowPhysicsNormals));
        }
    }

    if (input.keyPressed(Key::V)) {
        window->vSyncEnabled(!window->vSyncEnabled());
    }

    if (input.keyPressed(Key::Backslash)) {
        util::snapshot::SaveSnapshot(*window);
    }

    if (input.keyPressed(Key::Slash)) {
        window->cursorCaptured(!(window->cursorCaptured()));
    }

    if (input.keyPressed(Key::R)) {
        if (!window->recordingGIF()) {
            util::snapshot::StartGIFRecording(*window, {320, 240}, 8);
        }
        else {
            util::snapshot::StopGIFRecording(*window);
        }
    }

    // move camera

    if (auto pov = scene.visualWorld()->pointOfView().lock(); pov && window->cursorCaptured()) {

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

        static float MOVE_SPEED = 0;
        if (!MOVE_SPEED) {
            MOVE_SPEED = math::max(scene.rootNode()->extent());
        }

        float moveMultiplier = 1.0;
        if (input.keyDown(Key::LeftControl)) {
            moveMultiplier = 2.0;
        }

        if (input.keyDown(Key::W) || input.mouseButtonDown(MouseButton::Four)) {
            vec3 positionDelta = (float) info.deltaTime * MOVE_SPEED * moveMultiplier * camForward;
            pov->position(pov->position() + positionDelta);
        }
        else if (input.keyDown(Key::S)) {
            vec3 positionDelta = (float) info.deltaTime * MOVE_SPEED * moveMultiplier * -camForward;
            pov->position(pov->position() + positionDelta);
        }

        if (input.keyDown(Key::A)) {
            vec3 positionDelta = (float) info.deltaTime * MOVE_SPEED * moveMultiplier * -camRight;
            pov->position(pov->position() + positionDelta);
        }
        else if (input.keyDown(Key::D)) {
            vec3 positionDelta = (float) info.deltaTime * MOVE_SPEED * moveMultiplier * camRight;
            pov->position(pov->position() + positionDelta);
        }

        if (input.keyDown(Key::Space)) {
            float direction = 1;
            if (input.keyDown(Key::LeftShift)) {
                direction = -1;
            }
            vec3 positionDelta = (float) info.deltaTime * MOVE_SPEED * moveMultiplier * camUp;
            pov->position(pov->position() + positionDelta * direction);
        }
    }
}
