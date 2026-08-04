//
//  RenderGatherer.h
//  avara3d
//
//  Created by Morgan Davis on 12/24/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_RENDERGATHERER_H
#define AVARA3D_RENDER_RENDERGATHERER_H

#include "a3d/Math.h"
#include "a3d/scene/Scene.h"

namespace a3d {

    struct FrameStats;
    struct GatherOutput;

    class PhysicsWorld;
    class Scene;

    class RenderGatherer {

    public:
        /// Internal Static Member Functions ///

        static GatherOutput Gather(const Scene&               scene,
                                   const math::mat4&          view,
                                   const PhysicsWorld*        physicsWorld,
                                   const Scene::DebugOptions& debugOptions,
                                   FrameStats&                stats);
    };

}

#endif //AVARA3D_RENDER_RENDERGATHERER_H
