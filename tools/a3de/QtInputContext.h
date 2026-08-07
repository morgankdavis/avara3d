//
//  QtInputContext.h
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_QTINPUTCONTEXT_H
#define AVARA3D_QTINPUTCONTEXT_H

#include "a3d/input/DesktopInputContext.h"

namespace a3d::qt {

    class QtViewport;

    class QtInputContext : public DesktopInputContext {

    public:
        /// Public Lifecycle Functions ///

        // explicit QtInputContext(QtViewport& viewport);
        QtInputContext();

        /// Internal Member Functions ///

        void keyPressed(int qtKey, int modifiers);
        void keyReleased(int qtKey, int modifiers);
        void mouseMoved(float x, float y);
        void mouseButtonPressed(int qtButton);
        void mouseButtonReleased(int qtButton);
        void mouseWheelScrolled(int x, int y);

        /// InputContext Internal Member Functions ///

        void attachedToScene(Scene& scene) override;
        void visualWorldAttachedToScene(Scene& scene) override;

    private:
        /// Private Member Functions ///

        void        viewport(QtViewport* viewport);
        QtViewport* viewport() const;

        /// Private Member Variables ///

        QtViewport* _viewport;
    };

}

#endif //AVARA3D_QTINPUTCONTEXT_H
