//
//  InputContext.h
//  avara3d
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_INPUT_INPUTCONTEXT_H
#define AVARA3D_INPUT_INPUTCONTEXT_H

#include <cstdint>
#include <functional>

namespace a3d {

    class RenderContext;
    class Scene;

    class InputContext {

    public:
        // [Public Types]

        struct UpdateInfo {

            // zero-based enclosing host-update index
            std::uint64_t updateIndex {0};

            // monotonic seconds since the host update loop started,
            // measured at the beginning of this input update
            double        elapsedTime {0.0};

            // monotonic seconds since the beginning of the previous
            // host update; zero during the first update
            double        deltaTime {0.0};
        };

        using DidUpdateCallback = std::function<void(InputContext& inputContext, const UpdateInfo& info)>;

        // [Public Lifecycle Functions]

        InputContext();

        InputContext(const InputContext&)            = delete;
        InputContext& operator=(const InputContext&) = delete;

        InputContext(InputContext&&)            = delete;
        InputContext& operator=(InputContext&&) = delete;

        virtual ~InputContext();

        // [Public Member Functions]

        DidUpdateCallback didUpdateCallback() const;
        void              didUpdateCallback(DidUpdateCallback callback);

        // [Internal Member Functions]

        virtual void      update(const UpdateInfo& info)           = 0;
        virtual void      attachedToScene(Scene& scene)            = 0;
        virtual void      visualWorldAttachedToScene(Scene& scene) = 0;

    private:
        // [Private Member Variables]

        DidUpdateCallback _didUpdateCallback;
    };

}

#endif /* AVARA3D_INPUT_INPUTCONTEXT_H */
