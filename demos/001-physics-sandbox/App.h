//
//  App.h
//  001-physics-sandbox
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_DEMO_QUADROTOR_APP_H
#define AVARA3D_DEMO_QUADROTOR_APP_H

#include <memory>

#include "a3d/Application.h"

namespace a3d {

    class Node;
    class Scene;
    class Window;

}

namespace test::physicssandbox {

    class App : public a3d::Application {

    public:
        /// Public Lifecycle Functions ///

        App(int argc, char* argv[]);
        ~App() override;

        App(const App&) = delete;
        App& operator=(const App&) = delete;

        App(App&&) = delete;
        App& operator=(App&&) = delete;

    protected:
        /// Protected Member Functions ///

        std::unique_ptr<a3d::Scene> init() override;
        a3d::SimulationConfig       simulationConfig() const override;
        bool                        shouldContinue(const a3d::Scene& scene) override;
        void                        didShutdown() override;

        /// InputContext Callbacks ///

        void inputContextDidUpdate(a3d::InputContext&                   inputContext,
                                   const a3d::InputContext::UpdateInfo& info) override;

        /// Scene Callbacks ///

        void sceneWillStep(a3d::Scene& scene, const a3d::Scene::StepInfo& info) override;

    private:
        /// Private Member Variables ///

        std::unique_ptr<a3d::Window> _window;
        std::shared_ptr<a3d::Node>   _bananaNode;
    };

}

#endif // AVARA3D_DEMO_QUADROTOR_APP_H
