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
const double                          FIXED_TIMESTEP {1.0 / 120.0};
const bool                            DARK {false};

/// Public Lifecycle Functions ///

App::App(int argc, char* argv[]):
    Application(argc, argv, APP_LOG_LEVEL) {}

App::~App() = default;

/// Public Member Functions ///

std::unique_ptr<Scene> App::init() {
    try {
        _window = make_unique<Window>(RenderContext::RenderingApi::OpenGL, *util::filesystem::ExecutableName(),
                                      WINDOW_SIZE, FULLSCREEN, ENABLE_HIGH_DPI, ANTIALIAS_MODE);
        _window->vSyncEnabled(ENABLE_VSYNC);
        _window->cursorCaptured(CAPTURE_CURSOR);

        auto visualWorld = make_unique<VisualWorld>(*_window);

        auto physicsWorld = make_unique<PhysicsWorld>();

        auto mapPath = util::filesystem::AuxiliaryFilePath("Icebox", "alf");
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
    return {.timeStep = FIXED_TIMESTEP};
}

bool App::shouldContinue(const Scene& scene) {
    return _window->isOpen();
}

void App::didShutdown() {}

/// Runner Callbacks ///

void App::hostUpdate(Runner& runner, const Runner::UpdateInfo& info) {

    auto& scene = runner.scene();
    auto& inputContext = *scene.inputContext();

    Window* window = nullptr;
    if (scene.visualWorld()) {
        window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());
    }

    // get input

    auto im = static_cast<DesktopInputContext*>(&inputContext);

    auto mouseButtonsDown = im->mouseButtonsDown();
    auto keysDown = im->keysDown();
    auto keysPressed = im->keysPressed();
    auto mousePositionDelta = im->mousePositionDelta();

    using Key = DesktopInputContext::Key;
    using MouseButton = DesktopInputContext::MouseButton;

    if (keysPressed.count(Key::Escape)) {
        window->close();
    }

    if (keysPressed.count(Key::T)) {
        log::app::i()("TREE:\n{}", util::string::TreeString(*(scene.rootNode())));
    }

    if (keysPressed.count(Key::One)) {

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
    if (keysPressed.count(Key::Two)) {

        auto cameraNodes = vector<std::shared_ptr<Node>>();
        for (auto& node : scene.rootNode()->children(true)) {
            auto camera = node->camera();
            if (camera) {
                cameraNodes.push_back(node);
            }
        }

        scene.visualWorld()->pointOfView(cameraNodes[1]);
    }

    if (keysPressed.count(Key::Three)) {

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
    if (keysPressed.count(Key::P)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsBoundingBoxes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
                                                     DebugOptions::ShowPhysicsBoundingBoxes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(),
                                                  DebugOptions::ShowPhysicsBoundingBoxes));
        }
    }
    if (keysPressed.count(Key::G)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
                                                     DebugOptions::ShowPhysicsWireframes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes));
        }
    }
    if (keysPressed.count(Key::C)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsContactPoints)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
                                                     DebugOptions::ShowPhysicsContactPoints));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(),
                                                  DebugOptions::ShowPhysicsContactPoints));
        }
    }
    if (keysPressed.count(Key::N)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsNormals)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowPhysicsNormals));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowPhysicsNormals));
        }
    }

    if (keysPressed.count(Key::V)) {
        window->vSyncEnabled(!window->vSyncEnabled());
    }

    if (keysPressed.count(Key::Backslash)) {
        util::snapshot::SaveSnapshot(*window);
    }

    if (keysPressed.count(Key::Slash)) {
        window->cursorCaptured(!(window->cursorCaptured()));
    }

    const auto cursorCaptured = window->cursorCaptured();

    if (keysPressed.count(Key::R)) {
        if (!window->recordingGIF()) {
            util::snapshot::StartGIFRecording(*window, {320, 240}, 8);
        }
        else {
            util::snapshot::StopGIFRecording(*window);
        }
    }

    if (cursorCaptured) {

        // mouselook

        if (auto pov = scene.visualWorld()->pointOfView().lock()) {

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
            if (keysDown.count(Key::LeftControl)) {
                moveMultiplier = 2.0;
            }

            if (keysDown.count(Key::W) || mouseButtonsDown.count(MouseButton::Four)) {
                vec3 positionDelta = (float) info.deltaTime * MOVE_SPEED * moveMultiplier * camForward;
                pov->position(pov->position() + positionDelta);
            }
            else if (keysDown.count(Key::S)) {
                vec3 positionDelta = (float) info.deltaTime * MOVE_SPEED * moveMultiplier * -camForward;
                pov->position(pov->position() + positionDelta);
            }

            if (keysDown.count(Key::A)) {
                vec3 positionDelta = (float) info.deltaTime * MOVE_SPEED * moveMultiplier * -camRight;
                pov->position(pov->position() + positionDelta);
            }
            else if (keysDown.count(Key::D)) {
                vec3 positionDelta = (float) info.deltaTime * MOVE_SPEED * moveMultiplier * camRight;
                pov->position(pov->position() + positionDelta);
            }

            if (keysDown.count(Key::Space)) {
                float direction = 1;
                if (keysDown.count(Key::LeftShift)) {
                    direction = -1;
                }
                vec3 positionDelta = (float) info.deltaTime * MOVE_SPEED * moveMultiplier * camUp;
                pov->position(pov->position() + positionDelta * direction);
            }
        }
    }
}
