//
//  RenderGatherer.cc
//  avara3d
//
//  Created by Morgan Davis on 12/24/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
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

    const bool showBounds = util::bitmask::contains(debugOptions, Scene::DebugOptions::ShowBoundingBoxes);

    output.backgroundMaterial = scene.visualWorld()->backgroundMaterial();
    output.backgroundOrientation = scene.visualWorld()->backgroundOrientation();

    output.scene = &scene; // TODO: maybe change to AABB directly?

    if (showBounds) {
        DebugLinesBuilder::AppendAABB(output.debugLines, scene.aabb(false), *Color::Green());
    }

    while (!stack.empty()) {
        auto [n, parentWorld] = stack.back();
        stack.pop_back();

        ++stats.nodes;

        if (n->hidden()) {
            continue;
        }

        const mat4 world = parentWorld * n->transform();
        const bool wireframe = util::bitmask::contains(debugOptions, Scene::DebugOptions::ShowWireframes);

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
                item.transparent = (mat->blendFunction() != Material::BlendFunction::Disabled);

                // ! temnporary !
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
//				stats.numPolygons += element->faces().size();
                if (element->indexCount() > 0) {
                    stats.polygons += element->indexCount() / 3u;
                }
                else {
                    stats.polygons += element->vertexCount() / 3u;
                }
            }

            if (showBounds) {
                DebugLinesBuilder::AppendOBBFromLocalAABB(output.debugLines, mesh->localAABB(), world,
                                                          *Color::Gray());
                DebugLinesBuilder::AppendAABB(output.debugLines, mesh->worldAABB(world, false), *Color::Red());
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
        physicsWorld->appendDebugLines(output.debugLines, debugOptions);
    }

    return output;
}
