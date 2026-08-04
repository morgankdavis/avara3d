//
//  InputContext.h
//  avara3d
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_INPUT_INPUTCONTEXT_H
#define AVARA3D_INPUT_INPUTCONTEXT_H

namespace a3d {

    class RenderContext;
    class Scene;

    class InputContext {

    public:
        /// Public Lifecycle Functions ///

        InputContext();

        InputContext(const InputContext&)            = delete;
        InputContext& operator=(const InputContext&) = delete;

        InputContext(InputContext&&)            = delete;
        InputContext& operator=(InputContext&&) = delete;

        virtual ~InputContext();

        /// Internal Member Functions ///

        virtual void update()                                 = 0;
        virtual void attachedToScene(Scene& scene)            = 0;
        virtual void visualWorldAttachedToScene(Scene& scene) = 0;
    };

}

#endif /* AVARA3D_INPUT_INPUTCONTEXT_H */
