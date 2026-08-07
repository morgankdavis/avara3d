//
//  App.h
//  002-scenegraph
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TEST_SCENEGRAPH_APP_H
#define AVARA3D_TEST_SCENEGRAPH_APP_H

#include <memory>

#include "a3d/Application.h"

namespace a3d {

    class Scene;
    class Window;

}

namespace test::scenegraph {

    class App : public a3d::Application {

    public:
        /// Public Lifecycle Functions ///

        App(int argc, char* argv[]);
        ~App() override;

    protected:
        /// Protected Member Functions ///

        std::unique_ptr<a3d::Scene> init() override;
        a3d::SimulationConfig       simulationConfig() const override;
        bool                        shouldContinue(const a3d::Scene& scene) override;

        /// Runner Callbacks ///

        void hostUpdate(a3d::Runner&                   runner,
                        a3d::Scene&                    scene,
                        const a3d::Runner::UpdateInfo& info) override;

    private:
        /// Private Member Variables ///

        std::unique_ptr<a3d::Window> _window;
    };

}

#endif // AVARA3D_TEST_SCENEGRAPH_APP_H
