//
//  VisualWorld.cc
//  avara3d
//
//  Created by Morgan Davis on 11/25/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/VisualWorld.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>
#include <variant>

#include "a3d/Assert.h"
#include "a3d/Color.h"
#include "a3d/CubeImage.h"
#include "a3d/log/Log.h"
#include "a3d/mesh/AABB.h"
#include "a3d/mesh/IndexAccess.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/PrimitiveTopology.h"
#include "a3d/mesh/VertexAccess.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/profile/Profile.h"
#include "a3d/render/DrawPacket.h"
#include "a3d/render/DrawPacketizer.h"
#include "a3d/render/GatherOutput.h"
#include "a3d/render/RenderGatherer.h"
#include "a3d/render/Renderer.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/Flow.h"
#include "a3d/visual/material/Sampler.h"
#include "a3d/visual/material/Texture.h"
#include "a3d/visual/camera/PerspectiveCamera.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Private Types ///

struct HitTestCandidate {
    float         t;
    HitTestResult result;
};

/// Private Static Non-Member Prototypes ///

// tests whether the finite near-to-far picking segment intersects an axis-aligned bounding box. returns the
// earliest intersection position as a normalized segment parameter t in [0, 1], or nullopt if there is no hit.
static optional<float> IntersectSegmentAABB(const vec3& origin, const vec3& delta, const AABB& aabb);
// performs precise segment-versus-triangle test using Möller–Trumbore. deliberately two-sided and returns
// the intersection t in [0, 1], or nullopt.
static optional<float> IntersectSegmentTriangle(const vec3& origin,
                                                const vec3& delta,
                                                const vec3& a,
                                                const vec3& b,
                                                const vec3& c);
// transforms world-space picking segment into a node's mesh-local space, rejects elements using their AABBs,
// then tests their triangles. returns the nearest triangle hit anywhere in that Node's Mesh, packaged as a
// complete HitTestResult.
static optional<HitTestCandidate> IntersectNodeMesh(const shared_ptr<Node>& node,
                                                    const shared_ptr<Mesh>& mesh,
                                                    const mat4&             modelTransform,
                                                    const vec3&             worldOrigin,
                                                    const vec3&             worldDelta);

/// Public Lifecycle Functions ///

VisualWorld::VisualWorld(RenderContext& context):
    _background {},
    _backgroundMaterial {},

    _fog {},
    _atmosphericHaze {},
    _infiniteGround {},
    _defaultLightingEnabled {false},
    _pointOfView {},
    _renderContext {&context},
    _scene {},
    _didBeginFrameCallback {} {

    _renderContext->attachedToVisualWorld(this);
}

VisualWorld::~VisualWorld() {
    log::d()("Destroying VisualWorld {:p}", static_cast<void*>(this));

    if (_renderContext) {
        _renderContext->detachedFromVisualWorld(this);
    }
}

/// Public Member Functions ///

VisualWorld::Capabilities VisualWorld::capabilities() const {

    Capabilities capabilities {};

    if (_renderContext) {
        if (auto renderer = _renderContext->renderer()) {
            capabilities.wireframeRendering = renderer->capabilities().wireframeRendering;
        }
    }

    return capabilities;
}

optional<Background>& VisualWorld::background() {
    return _background;
}

void VisualWorld::background(const optional<Background>& background) {

    if (!background) {
        _background = {};
        _backgroundMaterial = nullptr;
        return;
    }

    const auto&          contents = background->contents();
    shared_ptr<Material> backgroundMaterial;

    if (auto texture = get_if<shared_ptr<Texture>>(&contents)) {

        if (!*texture) {
            throw invalid_argument("Background texture cannot be null.");
        }

        auto cubeImage = get_if<shared_ptr<CubeImage>>(&((*texture)->contents()));

        if (!cubeImage || !*cubeImage) {
            throw invalid_argument("Background texture must contain a CubeImage.");
        }

        auto sampler = (*texture)->sampler();
        sampler->wrapS(Sampler::WrapMode::ClampToEdge);
        sampler->wrapT(Sampler::WrapMode::ClampToEdge);
        sampler->wrapR(Sampler::WrapMode::ClampToEdge);

        backgroundMaterial = Material::EmissionMaterial(contents);
    }
    else if (auto color = get_if<shared_ptr<Color>>(&contents)) {

        if (!*color) {
            throw invalid_argument("Background color cannot be null.");
        }

        backgroundMaterial = Material::EmissionMaterial(contents);
    }
    else {
        throw invalid_argument("Background contents must be a Color or cubemap Texture.");
    }

    _background = background;
    _backgroundMaterial = std::move(backgroundMaterial);
}

const optional<Fog>& VisualWorld::fog() const {
    return _fog;
}

void VisualWorld::fog(const optional<Fog>& fog) {

    if (fog) {
        if (!fog->color) {
            throw invalid_argument("Fog color cannot be null.");
        }
        if (!isfinite(fog->startDistance) || fog->startDistance < 0.0f) {
            throw invalid_argument("Fog start distance must be finite and non-negative.");
        }
        if (!isfinite(fog->endDistance) || fog->endDistance <= fog->startDistance) {
            throw invalid_argument("Fog end distance must be finite and greater than fog start distance.");
        }
        if (!isfinite(fog->transitionExponent) || fog->transitionExponent < 0.0f) {
            throw invalid_argument("Fog transition exponent must be finite and non-negative.");
        }
    }
    _fog = fog;
}

const optional<AtmosphericHaze>& VisualWorld::atmosphericHaze() const {
    return _atmosphericHaze;
}

void VisualWorld::atmosphericHaze(const optional<AtmosphericHaze>& haze) {

    if (haze) {
        if (!haze->color) {
            throw invalid_argument("AtmosphericHaze color cannot be null.");
        }
        if (!isfinite(haze->baseHeight)) {
            throw invalid_argument("AtmosphericHaze base height must be finite.");
        }
        if (!isfinite(haze->density) || haze->density < 0.0f) {
            throw invalid_argument("AtmosphericHaze density must be finite and non-negative.");
        }
        if (!isfinite(haze->heightFalloff) || haze->heightFalloff <= 0.0f) {
            throw invalid_argument("AtmosphericHaze height falloff must be finite and greater than zero.");
        }
    }
    _atmosphericHaze = haze;
}

optional<InfiniteGround>& VisualWorld::infiniteGround() {
    return _infiniteGround;
}

void VisualWorld::infiniteGround(const optional<InfiniteGround>& ground) {

    if (ground) {

        if (!ground->color) {
            throw invalid_argument("InfiniteGround color cannot be null.");
        }

        if (!isfinite(ground->height)) {
            throw invalid_argument("InfiniteGround height must be finite.");
        }

        auto validateGrid = [](const InfiniteGround::Grid& grid, const char* name) {
            if (!grid.color) {
                throw invalid_argument(format("InfiniteGround {} grid color cannot be null.", name));
            }
            if (!isfinite(grid.spacing) || grid.spacing <= 0.0f) {
                throw invalid_argument(
                    format("InfiniteGround {} grid spacing must be finite and greater than zero.", name));
            }
            if (!isfinite(grid.lineWidthPixels) || grid.lineWidthPixels <= 0.0f) {
                throw invalid_argument(
                    format("InfiniteGround {} grid line width must be finite and greater than zero.", name));
            }
            if (!isfinite(grid.reliefStrength)) {
                throw invalid_argument(format("InfiniteGround {} grid relief strength must be finite.", name));
            }
        };

        if (ground->minorGrid) {
            validateGrid(*ground->minorGrid, "minor");
        }

        if (ground->majorGrid) {
            validateGrid(*ground->majorGrid, "major");
        }

        if (ground->curvature) {

            const auto& curvature = *ground->curvature;

            if (!isfinite(curvature.center.x) || !isfinite(curvature.center.y)) {
                throw invalid_argument("InfiniteGround curvature center must be finite.");
            }
            if (!isfinite(curvature.radius) || curvature.radius <= 0.0f) {
                throw invalid_argument("InfiniteGround curvature radius must be finite and greater than zero.");
            }
        }

        if (ground->radialFade) {

            const auto& fade = *ground->radialFade;

            if (!fade.color) {
                throw invalid_argument("InfiniteGround radial fade color cannot be null.");
            }
            if (!isfinite(fade.center.x) || !isfinite(fade.center.y)) {
                throw invalid_argument("InfiniteGround radial fade center must be finite.");
            }
            if (!isfinite(fade.startDistance) || fade.startDistance < 0.0f) {
                throw invalid_argument(
                    "InfiniteGround radial fade start distance must be finite and non-negative.");
            }
            if (!isfinite(fade.endDistance) || fade.endDistance <= fade.startDistance) {
                throw invalid_argument(
                    "InfiniteGround radial fade end distance must be finite and greater than start distance.");
            }
        }

        if (ground->horizonHaze) {

            const auto& haze = *ground->horizonHaze;

            if (!haze.color) {
                throw invalid_argument("InfiniteGround horizon haze color cannot be null.");
            }
            if (!isfinite(haze.angularWidthDegrees) || haze.angularWidthDegrees <= 0.0f
                || haze.angularWidthDegrees > 90.0f) {

                throw invalid_argument(
                    "InfiniteGround horizon haze angular width must be finite, greater than zero, and at most 90 degrees.");
            }
        }

        if (!isfinite(ground->specularIntensity) || ground->specularIntensity < 0.0f) {
            throw invalid_argument("InfiniteGround specular intensity must be finite and non-negative.");
        }

        if (!isfinite(ground->specularExponent) || ground->specularExponent <= 0.0f) {
            throw invalid_argument("InfiniteGround specular exponent must be finite and greater than zero.");
        }
    }

    _infiniteGround = ground;
}

weak_ptr<Node>& VisualWorld::pointOfView() {
    return _pointOfView;
}

void VisualWorld::pointOfView(const weak_ptr<Node>& cameraNode) {
    _pointOfView = cameraNode;
}

vec3 VisualWorld::projectPoint(const vec3& point) const {

    if (!_renderContext) {
        throw runtime_error("VisualWorld has no RenderContext.");
    }

    auto pov = _pointOfView.lock();
    if (!pov || !pov->camera()) {
        throw runtime_error("VisualWorld has no valid point of view.");
    }

    auto viewportSize = _renderContext->viewportLogicalSize();
    if (viewportSize.x == 0 || viewportSize.y == 0) {
        throw runtime_error("VisualWorld has an invalid viewport size.");
    }

    mat4 view = inverse(pov->worldTransform());
    mat4 proj = pov->camera()->projection(_renderContext->framebufferSize());
    vec4 clip = proj * view * vec4(point, 1.0f);

    if (math::abs(clip.w) <= F32_COMPARE_EPSILON) {
        throw runtime_error("Cannot project point with zero clip-space W.");
    }

    vec3 ndc {clip.x / clip.w, clip.y / clip.w, clip.z / clip.w};

    return {(ndc.x + 1.0f) * 0.5f * float(viewportSize.x), (1.0f - ndc.y) * 0.5f * float(viewportSize.y),
            (ndc.z + 1.0f) * 0.5f};
}

vec3 VisualWorld::unprojectPoint(const vec3& point) const {

    if (!_renderContext) {
        throw runtime_error("VisualWorld has no RenderContext.");
    }

    auto pov = _pointOfView.lock();
    if (!pov || !pov->camera()) {
        throw runtime_error("VisualWorld has no valid point of view.");
    }

    auto viewportSize = _renderContext->viewportLogicalSize();
    if (viewportSize.x == 0 || viewportSize.y == 0) {
        throw runtime_error("VisualWorld has an invalid viewport size.");
    }

    mat4 view = inverse(pov->worldTransform());
    mat4 proj = pov->camera()->projection(_renderContext->framebufferSize());

    vec4 ndc {(2.0f * point.x / float(viewportSize.x)) - 1.0f, 1.0f - (2.0f * point.y / float(viewportSize.y)),
              (2.0f * point.z) - 1.0f, 1.0f};

    vec4 world = inverse(proj * view) * ndc;

    if (math::abs(world.w) <= F32_COMPARE_EPSILON) {
        throw runtime_error("Cannot unproject point with zero homogeneous W.");
    }

    return {world.x / world.w, world.y / world.w, world.z / world.w};
}

vector<HitTestResult> VisualWorld::hitTest(const vec2& point) const {

    return hitTest(point, HitTestOptions {});
}

vector<HitTestResult> VisualWorld::hitTest(const vec2& point, const HitTestOptions& options) const {

    if (!isfinite(point.x) || !isfinite(point.y)) {
        throw invalid_argument("Hit-test point must be finite.");
    }

    if (!_scene) {
        throw runtime_error("VisualWorld has no Scene.");
    }

    const vec3 worldOrigin = unprojectPoint({point.x, point.y, 0.0f});
    const vec3 worldEnd = unprojectPoint({point.x, point.y, 1.0f});
    const vec3 worldDelta = worldEnd - worldOrigin;

    if (length(worldDelta) <= F32_COMPARE_EPSILON) {
        throw runtime_error("Hit-test segment is degenerate.");
    }

    struct TraversalEntry {
        shared_ptr<Node> node;
        mat4             parentWorld;
    };

    vector<TraversalEntry> stack;
    stack.reserve(256);
    stack.push_back({_scene->rootNode(), mat4 {1.0f}});

    optional<HitTestCandidate> closest;
    vector<HitTestCandidate>   all;

    while (!stack.empty()) {

        auto entry = std::move(stack.back());
        stack.pop_back();

        if (entry.node->hidden()) {
            continue;
        }

        const mat4 modelTransform = entry.parentWorld * entry.node->transform();

        if (const auto& mesh = entry.node->mesh()) {

            auto candidate = IntersectNodeMesh(entry.node, mesh, modelTransform, worldOrigin, worldDelta);

            if (candidate) {

                switch (options.searchMode) {

                    case HitTestSearchMode::Any:
                        return {std::move(candidate->result)};

                    case HitTestSearchMode::Closest:
                        if (!closest || candidate->t < closest->t) {
                            closest = std::move(*candidate);
                        }
                        break;

                    case HitTestSearchMode::All:
                        all.push_back(std::move(*candidate));
                        break;
                }
            }
        }

        for (const auto& child : entry.node->children()) {
            stack.push_back({child, modelTransform});
        }
    }

    switch (options.searchMode) {

        case HitTestSearchMode::Any:
            return {};

        case HitTestSearchMode::Closest:
            if (closest) {
                return {std::move(closest->result)};
            }
            return {};

        case HitTestSearchMode::All:
            sort(all.begin(), all.end(), [](const HitTestCandidate& a, const HitTestCandidate& b) {
                return a.t < b.t;
            });

            vector<HitTestResult> results;
            results.reserve(all.size());

            for (auto& candidate : all) {
                results.push_back(std::move(candidate.result));
            }

            return results;
    }

    return {};
}

bool VisualWorld::defaultLightingEnabled() const {
    return _defaultLightingEnabled;
}

void VisualWorld::defaultLightingEnabled(bool enabled) {
    _defaultLightingEnabled = enabled;
}

RenderContext* VisualWorld::renderContext() const {
    return _renderContext;
}

Scene* VisualWorld::scene() const {
    return _scene;
}

/// Internal Member Functions ///

void VisualWorld::attachedToScene(Scene& scene) {
    log::t()("scene: {:p}", static_cast<void*>(&scene));

    _scene = &scene;
}

void VisualWorld::detachedFromScene(Scene& scene) {
    log::t()("scene: {:p}", static_cast<void*>(&scene));

    _scene = nullptr;
}

VisualWorld::DidBeginFrameCallback VisualWorld::didBeginFrameCallback() const {
    return _didBeginFrameCallback;
}

void VisualWorld::didBeginFrameCallback(DidBeginFrameCallback function) {
    _didBeginFrameCallback = function;
}

bool VisualWorld::draw(const Scene&             scene,
                       const PhysicsWorld*      physicsWorld,
                       const RenderInfo&        info,
                       Scene::DebugOptions      debugOptions,
                       FrameStats&              stats,
                       Profiler&                profiler,
                       const FrameStatsHistory& statsHistory) {
    if (!util::flow::edge_guard(_renderContext, [&] {
            log::e()("No RenderContext attached to VisualWorld {:p}", static_cast<void*>(this));
        })) {
        return false;
    }

    auto renderer = _renderContext->renderer();
    if (!util::flow::edge_guard(renderer, [&] {
            log::e()("No Renderer attached to RenderContext {:p}", static_cast<void*>(_renderContext));
        })) {
        return false;
    }

    util::flow::once([&] {
        firstDraw();
    });

    prof::profile(profiler, Profiler::Tag::EngineCpu, [&] {
        // there is some "RenderCpu" type stuff bundled in here for GLFWWindow and QtViewport
        _renderContext->beginFrame(scene);
    });

    prof::profile(profiler, Profiler::Tag::RenderCpu, [&] {
        renderer->beginFrame(scene, *_renderContext, debugOptions, stats, profiler);
    });

    if (auto didBeginFrame = didBeginFrameCallback()) {
        prof::profile(profiler, Profiler::Tag::Application, [&] {
            didBeginFrame(*this, info);
        });
    }

    auto pov = pointOfView().lock();
    auto camera = pov ? pov->camera() : nullptr;
    bool povValid = true;

    if (!pov) {
        log::e()("No point of view!");
        povValid = false;
    }
    else if (pov->scene() != &scene) {
        log::e()("Point of view not in our scene!");
        povValid = false;
    }
    else if (!camera) {
        log::e()("Point of view has no camera!");
        povValid = false;
    }

    if (povValid) {

        auto [view, proj] = prof::profile(profiler, Profiler::Tag::EngineCpu, [&] {
            return std::pair {inverse(pov->worldTransform()),
                              pov->camera()->projection(_renderContext->framebufferSize())};
        });

        prof::profile(profiler, Profiler::Tag::RenderCpu, [&] {
            renderer->preTraversal(scene, *_renderContext, debugOptions, stats);

            auto gatherItems = RenderGatherer::Gather(scene, view, physicsWorld, debugOptions, stats);

            renderer->postTraversal(scene, *_renderContext, view, gatherItems.lightNodes, debugOptions, stats);

            auto packet = DrawPacketizer::Packetize(gatherItems);

            Renderer::FrameParams params = {*_renderContext, view, proj, debugOptions, &stats, &profiler};

            renderer->renderPacket(packet, params);
        });
    }
    else {
        renderer->clear(Renderer::ClearCommand {}, *_renderContext);
    }

    prof::profile(profiler, Profiler::Tag::RenderCpu, [&] {
        renderer->endFrame(scene, *_renderContext, debugOptions, stats, profiler, statsHistory);
    });

    prof::profile(profiler, Profiler::Tag::EngineCpu, [&] {
        // there is some "RenderCpu" type stuff bundled in here for GLFWWindow and QtViewport
        _renderContext->endFrame(scene);
    });

    prof::profile(profiler, Profiler::Tag::RenderCpu, [&] {
        _renderContext->swapBuffers();
    });

    prof::profile(profiler, Profiler::Tag::EngineCpu, [&] {
        if (_renderContext->recordingGIF()) {
            _renderContext->saveGIFFrame(info.updateDeltaTime);
        }
    });

    return true;
}

shared_ptr<Material> VisualWorld::backgroundMaterial() {
    return _backgroundMaterial;
}

/// Private Member Functions ///

void VisualWorld::firstDraw() {

    // check for or create a POV

    if (!_pointOfView.lock()) {

        // try to assign a POV from the scene
        for (auto& node : _scene->rootNode()->children(true)) {
            if (node->camera()) {
                log::i()("Setting {:p} as POV.", static_cast<void*>(node.get()));
                _pointOfView = node;
                break;
            }
        }
    }

    if (!_pointOfView.lock()) {

        // still no POV. add a default one.
        log::i()("Adding default POV.");
        auto pov = defaultPOV();
        _scene->rootNode()->addChild(pov);
        _pointOfView = pov;
    }
}

shared_ptr<Node> VisualWorld::defaultPOV() {

    if (!_scene) {
        log::w()("Can't create default camera: scene is null.");
        return {};
    }

    auto camera = make_shared<PerspectiveCamera>();
    camera->name("Default Camera");

    auto aabb = _scene->rootNode()->aabb();
    vec3 center = (aabb.min + aabb.max) * 0.5f;
    vec3 extents = (aabb.max - aabb.min) * 0.5f;

    auto  fbSize = _renderContext->framebufferSize();
    float aspect = float(fbSize.x) / float(fbSize.y);

    float vFov = camera->yFov();
    float hFov = 2.0f * math::atan(math::tan(vFov * 0.5f) * aspect);

    float distH = extents.x / math::tan(hFov * 0.5f);
    float distV = extents.y / math::tan(vFov * 0.5f);

    float dist = math::max(distH, distV) + extents.z;

    vec3 eye = center + vec3(0, 0, dist);
    mat4 view = math::look_at(eye, center, vec3(0, 1, 0));

    auto cameraNode = make_shared<Node>();
    cameraNode->transform(inverse(view));

    cameraNode->camera(camera);

    return cameraNode;
}

/// Private Static Non-Member Functions ///

optional<float> IntersectSegmentAABB(const vec3& origin, const vec3& delta, const AABB& aabb) {

    if (!aabb.valid()) {
        return {};
    }

    float tMin = 0.0f;
    float tMax = 1.0f;

    for (int axis = 0; axis < 3; ++axis) {

        if (math::abs(delta[axis]) <= F32_COMPARE_EPSILON) {

            if (origin[axis] < aabb.min[axis] || origin[axis] > aabb.max[axis]) {
                return {};
            }

            continue;
        }

        const float inverseDelta = 1.0f / delta[axis];

        const float t0 = (aabb.min[axis] - origin[axis]) * inverseDelta;
        const float t1 = (aabb.max[axis] - origin[axis]) * inverseDelta;

        const float tNear = math::min(t0, t1);
        const float tFar = math::max(t0, t1);

        tMin = math::max(tMin, tNear);
        tMax = math::min(tMax, tFar);

        if (tMin > tMax) {
            return {};
        }
    }

    return tMin;
}

optional<float> IntersectSegmentTriangle(const vec3& origin,
                                         const vec3& delta,
                                         const vec3& a,
                                         const vec3& b,
                                         const vec3& c) {

    const vec3 edgeAB = b - a;
    const vec3 edgeAC = c - a;

    const vec3  p = cross(delta, edgeAC);
    const float determinant = dot(edgeAB, p);

    // Using abs() here deliberately makes the test two-sided.
    if (math::abs(determinant) <= F32_COMPARE_EPSILON) {
        return {};
    }

    const float inverseDeterminant = 1.0f / determinant;
    const vec3  fromA = origin - a;

    const float u = dot(fromA, p) * inverseDeterminant;
    if (u < 0.0f || u > 1.0f) {
        return {};
    }

    const vec3 q = cross(fromA, edgeAB);

    const float v = dot(delta, q) * inverseDeterminant;
    if (v < 0.0f || u + v > 1.0f) {
        return {};
    }

    const float t = dot(edgeAC, q) * inverseDeterminant;
    if (t < 0.0f || t > 1.0f) {
        return {};
    }

    return t;
}

optional<HitTestCandidate> IntersectNodeMesh(const shared_ptr<Node>& node,
                                             const shared_ptr<Mesh>& mesh,
                                             const mat4&             modelTransform,
                                             const vec3&             worldOrigin,
                                             const vec3&             worldDelta) {

    const mat3  linearTransform {modelTransform};
    const float linearDeterminant = determinant(linearTransform);

    if (!isfinite(linearDeterminant) || linearDeterminant == 0.0f) {
        return {};
    }

    const mat4 inverseModelTransform = inverse(modelTransform);

    const vec3 worldEnd = worldOrigin + worldDelta;

    const vec3 localOrigin = vec3 {inverseModelTransform * vec4 {worldOrigin, 1.0f}};
    const vec3 localEnd = vec3 {inverseModelTransform * vec4 {worldEnd, 1.0f}};
    const vec3 localDelta = localEnd - localOrigin;

    optional<float>    bestT;
    const MeshElement* bestElement = nullptr;
    optional<uint32_t> bestFaceIndex;
    vec3               bestA;
    vec3               bestB;
    vec3               bestC;

    for (const auto& elementPtr : mesh->elements()) {

        const auto& element = *elementPtr;

        if (element.topology() != PrimitiveTopology::Triangles) {
            continue;
        }

        const AABB localAABB = element.localAABB();
        if (!localAABB.valid()) {
            continue;
        }

        const auto aabbHit = IntersectSegmentAABB(localOrigin, localDelta, localAABB);
        if (!aabbHit) {
            continue;
        }

        // The nearest possible hit in this element is already farther than
        // the best triangle found so far.
        if (bestT && *aabbHit > *bestT) {
            continue;
        }

        const auto positions = VertexAccess::GetPositionStreamView(element);
        if (!positions) {
            continue;
        }

        uint32_t faceIndex = 0;

        IndexAccess::ForEachTriangle(element, [&](uint32_t indexA, uint32_t indexB, uint32_t indexC) {
            const uint32_t currentFaceIndex = faceIndex++;

            const bool indicesValid =
                indexA < positions->count && indexB < positions->count && indexC < positions->count;

            A3D_ASSERT(indicesValid);

            if (!indicesValid) {
                return;
            }

            const vec3 a = VertexAccess::ReadVec3(VertexBaseAt(*positions, indexA), positions->offset);
            const vec3 b = VertexAccess::ReadVec3(VertexBaseAt(*positions, indexB), positions->offset);
            const vec3 c = VertexAccess::ReadVec3(VertexBaseAt(*positions, indexC), positions->offset);

            const auto t = IntersectSegmentTriangle(localOrigin, localDelta, a, b, c);

            if (!t || (bestT && *t >= *bestT)) {
                return;
            }

            bestT = *t;
            bestElement = &element;
            bestFaceIndex = currentFaceIndex;
            bestA = a;
            bestB = b;
            bestC = c;
        });
    }

    if (!bestT || !bestElement || !bestFaceIndex) {
        return {};
    }

    const vec3 localCoordinates = localOrigin + localDelta * *bestT;
    const vec3 worldCoordinates = worldOrigin + worldDelta * *bestT;

    const vec3 localNormal = normalize(cross(bestB - bestA, bestC - bestA));

    const mat3 normalTransform {transpose(inverseModelTransform)};
    const vec3 worldNormal = normalize(normalTransform * localNormal);

    return HitTestCandidate {
        .t = *bestT,
        .result =
            HitTestResult {
                node,
                mesh,
                bestElement,
                bestFaceIndex,
                localCoordinates,
                worldCoordinates,
                localNormal,
                worldNormal,
                modelTransform,
            },
    };
}
