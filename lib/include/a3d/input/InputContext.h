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

    /**
     * @brief Base interface for Scene input state updated once per Runner host update.
     *
     * Event polling occurs before the InputContext is updated. The did-update callback
     * runs after the concrete input context has made the current update's state
     * available and before simulation scheduling and rendering continue.
     */
    class InputContext {

    public:
        // [Public Types]

        /** @brief Timing information for one input update. */
        struct UpdateInfo {

            // zero-based enclosing host-update index
            std::uint64_t updateIndex {0}; ///< Zero-based index of the enclosing Runner host update.

            // monotonic seconds since the host update loop started,
            // measured at the beginning of this input update
            double        elapsedTime {0.0}; ///< Elapsed host-loop time at this input update, in seconds.

            // monotonic seconds since the beginning of the previous
            // host update; zero during the first update
            double        deltaTime {0.0}; ///< Time since the preceding host update, in seconds; zero on the first update.
        };

        /** @brief Callback invoked after input state has been updated for the current Runner host update. */
        using DidUpdateCallback = std::function<void(InputContext& inputContext, const UpdateInfo& info)>;

        // [Public Lifecycle Functions]

        /** @brief Creates an InputContext with no did-update callback. */
        InputContext();

        InputContext(const InputContext&)            = delete;
        InputContext& operator=(const InputContext&) = delete;

        InputContext(InputContext&&)            = delete;
        InputContext& operator=(InputContext&&) = delete;

        virtual ~InputContext();

        // [Public Member Functions]

        /** @brief Returns the callback invoked after each input update. */
        DidUpdateCallback didUpdateCallback() const;

        /** @brief Sets the did-update callback; an empty callback disables it. */
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
