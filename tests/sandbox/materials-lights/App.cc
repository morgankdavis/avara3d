//
//  App.cc
//  materials-lights
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::matlights;
using namespace std;

/// Private Static Non-Member Prototypes ///

void SetAllFilterModes(Sampler::FilterMode mode, Scene& scene);
void SetAllMaxAnisotropy(float anisotropy, Scene& scene);

/// Private Constants ///

const Log::Level                  APP_LOG_LEVEL {Log::Level::Debug};
const uvec2                       WINDOW_SIZE {1280, 768};
const bool                        FULLSCREEN {false};
const bool                        ENABLE_HIGH_DPI {true};
const RenderContext::Antialiasing ANTIALIASING {RenderContext::Antialiasing::Msaa4X};
const bool                        ENABLE_VSYNC {false};
const bool                        CAPTURE_CURSOR {false};
const bool                        ORTHO_CAMERA {false};

const float BACKGROUND_ROTATION_SPEED {radians(1.0)};
const vec3  BACKGROUND_ROTATION_AXIS {0.258819f, 0.965926f, 0.0f};

/// Public Lifecycle Functions ///

App::App(int argc, char* argv[]):
    Application(argc, argv, APP_LOG_LEVEL),
    _pointLightNode {},
    _pointLightOrbitRadius {0.0f} {}

App::~App() = default;

/// Application Protected Member Functions ///

std::unique_ptr<Scene> App::init() {
    try {
        _window = make_unique<Window>(RenderContext::RenderingApi::OpenGL, *util::fs::ExecutableName(),
                                      WINDOW_SIZE, FULLSCREEN, ENABLE_HIGH_DPI, ANTIALIASING);
        _window->vSyncEnabled(ENABLE_VSYNC);
        _window->cursorCaptured(CAPTURE_CURSOR);

        auto visualWorld = make_unique<VisualWorld>(*_window);
        visualWorld->fog(Fog {
            .color = Color::LightGray(),
            .startDistance = 500.0f,
            .endDistance = 5000.0f,
            .transitionExponent = 1.0f,
        });
        //visualWorld->usesDefaultLighting(true);
        visualWorld->background(Background {
            make_shared<Texture>(std::move(util::fs::CubeImageNamed("nebula1_blue")))});

        auto scene = util::fs::SceneNamed("cat_island/cat_island", Scene::ImportOptions::ImportMeshes
                                                                       | Scene::ImportOptions::ImportMaterials
                                                                       | Scene::ImportOptions::ImportCameras);

        scene->visualWorld(std::move(visualWorld));
        scene->inputContext(std::move(Window::InputContext()));
        scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

        auto ambientLight = make_shared<AmbientLight>(make_shared<Color>(0.2f));
        ambientLight->name("ambient");
        auto ambientLightNode = Node::LightNode(ambientLight);
        scene->rootNode()->addChild(ambientLightNode);

        auto pointLight = make_shared<PointLight>(Color::White());
        pointLight->name("point");
        pointLight->attenuation(Attenuation {.quadratic = 0.002f});
        auto pointLightNode = Node::LightNode(pointLight);
        auto material = make_shared<Material>();
        material->name("LIGHT material");
        material->emission(Color::White());
        auto geometry = Sphere::Mesh(1.5, 4, material);
        pointLightNode->mesh(geometry);
        scene->rootNode()->addChild(pointLightNode);
        _pointLightNode = pointLightNode;

        if (ORTHO_CAMERA) {

            auto frustum = scene->rootNode()->aabb();
            frustum.min.z = 0.01;
            frustum.max.z = 10000;
            auto orthoCameraNode = Node::CameraNode(make_shared<OrthographicCamera>("Ortho camera", frustum));

            scene->rootNode()->addChild(orthoCameraNode);

            // find the imported camera node, get its world transform, and apply it to our ortho camera
            for (auto& node : scene->rootNode()->children(true)) {
                if (node->camera()) {
                    orthoCameraNode->transform(node->worldTransform());
                    auto pos = orthoCameraNode->position();
                    pos.y = 0;
                    orthoCameraNode->position(pos);
                    scene->visualWorld()->pointOfView(orthoCameraNode);
                    break;
                }
            }
        }

        _pointLightOrbitRadius = math::max(scene->rootNode()->extent()) * 0.46f;

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

bool App::shouldContinue(const Scene& scene) {
    return _window->isOpen();
}

void App::inputDidUpdate(Runner&                         runner,
                         Scene&                          scene,
                         InputContext&                   inputContext,
                         const InputContext::UpdateInfo& info) {

    auto window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());

    // get input

    auto input = static_cast<DesktopInputContext*>(&inputContext);

    auto mouseScrollWheelDelta = input->mouseScrollWheelDelta();

    using Key = DesktopInputContext::Key;

    if (input->keyPressed(Key::Escape)) {
        window->close();
    }

    if (input->keyPressed(Key::T)) {
        log::app::i()("TREE:\n{}", util::string::TreeString(*(scene.rootNode())));
    }

    using FilterMode = Sampler::FilterMode;
    const bool nearestPressed = input->keyPressed(Key::One);
    const bool linearPressed = input->keyPressed(Key::Two);
    const bool nearestMipmapNearestPressed = input->keyPressed(Key::Three);
    const bool nearestMipmapLinearPressed = input->keyPressed(Key::Four);
    const bool linearMipmapNearestPressed = input->keyPressed(Key::Five);
    const bool linearMipmapLinearPressed = input->keyPressed(Key::Six);

    if (nearestPressed) {
        SetAllFilterModes(FilterMode::Nearest, scene);
    }
    else if (linearPressed) {
        SetAllFilterModes(FilterMode::Linear, scene);
    }
    else if (nearestMipmapNearestPressed) {
        SetAllFilterModes(FilterMode::NearestMipmapNearest, scene);
    }
    else if (nearestMipmapLinearPressed) {
        SetAllFilterModes(FilterMode::NearestMipmapLinear, scene);
    }
    else if (linearMipmapNearestPressed) {
        SetAllFilterModes(FilterMode::LinearMipmapNearest, scene);
    }
    else if (linearMipmapLinearPressed) {
        SetAllFilterModes(FilterMode::LinearMipmapLinear, scene);
    }

    const bool minAnisotropyPressed = input->keyPressed(Key::LeftBracket);
    const bool maxAnisotropyPressed = input->keyPressed(Key::RightBracket);

    if (minAnisotropyPressed) {
        SetAllMaxAnisotropy(1, scene);
    }
    else if (maxAnisotropyPressed) {
        SetAllMaxAnisotropy(16, scene);
    }

    using DebugOptions = Scene::DebugOptions;

    if (input->keyPressed(Key::F)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowWireframes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowWireframes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowWireframes));
        }
    }

    if (input->keyPressed(Key::B)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowBoundingBoxes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
        }
    }

    if (input->keyPressed(Key::I)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowStatsOverlay)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
        }
    }

    if (input->keyPressed(Key::V)) {
        window->vSyncEnabled(!(window->vSyncEnabled()));
    }

    if (input->keyPressed(Key::Backslash)) {
        util::snapshot::SaveSnapshot(*window);
    }

    if (input->keyPressed(Key::R)) {
        if (!window->recordingGIF()) {
            util::snapshot::StartGIFRecording(*window, {320, 240}, 8);
        }
        else {
            util::snapshot::StopGIFRecording(*window);
        }
    }

    if (input->keyPressed(Key::Slash)) {
        window->cursorCaptured(!(window->cursorCaptured()));
    }

    if (auto pov = scene.visualWorld()->pointOfView().lock(); pov && _window->cursorCaptured()) {

        if (mouseScrollWheelDelta.y) {

            static const float FOV_SPEED = 2.5; // degrees/roll

            auto camera = dynamic_pointer_cast<PerspectiveCamera>(pov->camera());
            auto fov = camera->yFov();
            fov += mouseScrollWheelDelta.y * -radians(FOV_SPEED);
            camera->yFov(fov);
        }

        _cameraController.update(*pov, *input, info.deltaTime);
    }
}

void App::sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    if (auto pointLight = _pointLightNode.lock()) {
        const vec3  center {0.0f, 30.0f, 0.0f};
        const float angle = radians(30.0f) * static_cast<float>(info.endTime);
        const float x = math::sin(angle) * _pointLightOrbitRadius;
        const float y = math::cos(angle) * _pointLightOrbitRadius;
        pointLight->position(center + vec3 {x, y, -x});
    }
}

void App::frameDidBegin(Runner&, Scene&, VisualWorld& visualWorld, const VisualWorld::RenderInfo& info) {

    const float delta = static_cast<float>(info.updateDeltaTime);

    static float angle = 0;
    angle += delta * BACKGROUND_ROTATION_SPEED;

    if (auto& background = visualWorld.background()) {
        background->orientation(quaternion(BACKGROUND_ROTATION_AXIS, angle));
    }
}

/// Private Static Non-Member Functions ///

void SetAllFilterModes(Sampler::FilterMode mode, Scene& scene) {

    log::app::i()("SetAllFilterModes: {}", (unsigned) mode);

    for (auto& node : scene.rootNode()->children(true)) {

        auto geometry = node->mesh();
        if (geometry) {

            for (auto& material : geometry->materials()) {

                for (auto& [property, type] : material->properties()) {

                    if (auto texture = get_if<shared_ptr<Texture>>(property)) {
                        auto sampler = (*texture)->sampler();
                        sampler->minificationFilter(mode);
                        sampler->magnificationFilter(mode);
                    }
                }
            }
        }
    }
}

void SetAllMaxAnisotropy(float anisotropy, Scene& scene) {

    log::app::i()("SetAllMaxAnisotropy: {}", anisotropy);

    for (auto& node : scene.rootNode()->children(true)) {

        auto geometry = node->mesh();
        if (geometry) {

            for (auto& material : geometry->materials()) {

                for (auto& [property, type] : material->properties()) {

                    if (auto texture = get_if<shared_ptr<Texture>>(property)) {
                        auto sampler = (*texture)->sampler();
                        sampler->maxAnisotropy(anisotropy);
                    }
                }
            }
        }
    }
}
