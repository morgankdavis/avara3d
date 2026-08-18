//
//  RenderGatherer.cc
//  avara3d
//
//  Created by Morgan Davis on 12/24/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/RenderGatherer.h"

#include "a3d/Color.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/profile/FrameStats.h"
#include "a3d/render/DebugLinesBuilder.h"
#include "a3d/render/GatherOutput.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/Bitmask.h"
#include "a3d/visual/VisualWorld.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Private Constants ///

const Color MESH_OBB_COLOR {vec4 {0.5f, 0.5f, 0.5f, 1.0f}};
const Color MESH_AABB_COLOR {vec4 {1.0f, 0.0f, 0.0f, 1.0f}};
const Color SCENE_AABB_COLOR {vec4 {0.0f, 0.5f, 0.0f, 1.0f}};
const Color HIGHLIGHT_BOX_COLOR {vec4 {1.0f, 1.0f, 0.0f, 1.0f}};
const vec4 HIGHLIGHT_TINT_COLOR {1.0f, 1.0f, 0.0f, 0.5f};
static constexpr float MESH_DEBUG_FRAME_SIZE = 0.5f;

/// Internal Static Member Functions ///

// "gather / collect / cull"
GatherOutput RenderGatherer::Gather(const Scene&               scene,
                                    const math::mat4&          view,
                                    const PhysicsWorld*        physicsWorld,
                                    const Scene::DebugOptions& debugOptions,
                                    FrameStats&                stats) {

    GatherOutput output {};
    output.renderItems.reserve(1024);
    output.lightNodes.reserve(64);

    struct GatherEntry {
        Node*      node;
        math::mat4 parentWorld;
    };

    vector<GatherEntry> stack;
    stack.reserve(256);
    stack.push_back({scene.rootNode().get(), mat4(1.0)});

    const bool showMeshBounds = util::bitmask::contains(debugOptions, Scene::DebugOptions::ShowMeshBounds);
    const bool showMeshFrames = util::bitmask::contains(debugOptions, Scene::DebugOptions::ShowMeshFrames);

    output.backgroundMaterial = scene.visualWorld()->backgroundMaterial();
    if (const auto& background = scene.visualWorld()->background()) {
        output.backgroundOrientation = background->orientation;
    }

    output.ground = scene.visualWorld()->ground();

    output.scene = &scene; // TODO: maybe change to AABB directly?

    while (!stack.empty()) {
        auto [n, parentWorld] = stack.back();
        stack.pop_back();

        ++stats.nodes;

        if (n->hidden()) {
            continue;
        }

        const mat4 world = parentWorld * n->transform();
        const bool wireframe = util::bitmask::contains(debugOptions, Scene::DebugOptions::ShowMeshWireframes);
        const bool showHighlightBox =
            util::bitmask::contains(n->debugOptions(), Node::DebugOptions::ShowHighlightBox);
        const bool showHighlightTint =
            util::bitmask::contains(n->debugOptions(), Node::DebugOptions::ShowHighlightTint);

        if (auto* mesh = n->mesh().get()) {

            const auto& elements = mesh->elements();
            const auto& materials = mesh->materials();

            for (size_t e = 0; e < elements.size(); ++e) {
                auto* element = elements[e].get();

                Material* mat = nullptr;
                if (!materials.empty()) {
                    const size_t index = e % materials.size();
                    mat = materials[index].get();
                }
                if (!mat) {
                    mat = Material::DefaultMaterial().get();
                }

                RenderItem item;
                item.mesh = mesh;
                item.layout = element->vertexLayout();
                item.elementIndex = e;
                item.element = element;
                item.material = mat;
                item.model = world;
                item.aabb = element->worldAABB(world, false);
                item.renderOrder = n->renderOrder();
                item.transparent = (mat->blendFunction() != Material::BlendFunction::Disabled);

                if (showHighlightTint) {
                    item.tint = HIGHLIGHT_TINT_COLOR;
                }

                // ! TEMPORARY !
                if (wireframe) {
                    item.style = RenderStyle::Wireframe; // TODO: test WireframeOverlay
                }
                else {
                    item.style = RenderStyle::Normal;
                }

                // depth: view-space Z of bounds center
                const vec3 center = (item.aabb.min + item.aabb.max) / 2.0f;
                const vec4 vpos = view * vec4(center, 1.0f);
                item.depth = vpos.z;

                output.renderItems.push_back(item);

                ++stats.elements;
                if (element->indexCount() > 0) {
                    stats.polygons += element->indexCount() / 3u;
                }
                else {
                    stats.polygons += element->vertexCount() / 3u;
                }
            }

            if (showMeshBounds) {

                if (!showHighlightBox) {
                    DebugLinesBuilder::AppendOBBFromLocalAABB(output.debugLines, mesh->localAABB(), world,
                                                              MESH_OBB_COLOR);
                }

                DebugLinesBuilder::AppendAABB(output.debugLines, mesh->worldAABB(world, false),
                                              MESH_AABB_COLOR);
            }

            if (showMeshFrames) {
                DebugLinesBuilder::AppendFrame(output.debugLines, world, MESH_DEBUG_FRAME_SIZE);
            }

            if (showHighlightBox) {
                DebugLinesBuilder::AppendOBBFromLocalAABB(output.debugLines, mesh->localAABB(), world,
                                                          HIGHLIGHT_BOX_COLOR);
            }

            ++stats.meshes;
        }

        if (n->light()) {
            output.lightNodes.push_back(n);
            ++stats.lights;
        }

        for (const auto& c : n->children()) {
            stack.push_back({c.get(), world});
        }
    }

    if (physicsWorld) {
        physicsWorld->appendDebugLines(output.debugLines);
    }

    if (showMeshBounds) {
        // TODO: maybe give this its own debug option
        DebugLinesBuilder::AppendAABB(output.debugLines, scene.aabb(false), SCENE_AABB_COLOR);
    }

    return output;
}
