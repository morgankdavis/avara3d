//
//  App.h
//  input-camera
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TEST_INPUTCAMERA_APP_H
#define AVARA3D_TEST_INPUTCAMERA_APP_H

#include <memory>

#include "a3d/Application.h"
#include "a3d/extension/camera/FlyCameraController.h"

namespace a3d {

    class PhysicsShape;
    class Scene;
    class Window;

}

namespace test::inputcam {

    class App : public a3d::Application {

    public:
        // [Public Lifecycle Functions]

        App(int argc, char* argv[]);
        ~App() override;

    protected:
        // [Application Protected Member Functions]

        std::unique_ptr<a3d::Scene> init() override;
        bool                        shouldContinue(const a3d::Scene& scene) override;

        void inputDidUpdate(a3d::Runner&                         runner,
                            a3d::Scene&                          scene,
                            a3d::InputContext&                   inputContext,
                            const a3d::InputContext::UpdateInfo& info) override;

    private:
        // [Private Member Variables]

        std::unique_ptr<a3d::Window>  _window;
        a3d::ext::FlyCameraController _cameraController;
    };

}

#endif // AVARA3D_TEST_INPUTCAMERA_APP_H
