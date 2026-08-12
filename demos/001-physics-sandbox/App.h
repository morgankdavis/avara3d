//
//  App.h
//  001-physics-sandbox
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_DEMO_PHYSICSSANDBOX_APP_H
#define AVARA3D_DEMO_PHYSICSSANDBOX_APP_H

#include <chrono>
#include <memory>
#include <optional>

#include "a3d/Application.h"
#include "a3d/Math.h"
#include "a3d/extension/camera/TurntableCameraController.h"

namespace a3d {

    class Node;
    class Scene;
    class Window;

}

namespace demo::physicssandbox {

    class App : public a3d::Application {

    public:
        /// Public Types ///

        struct PickResult {
            std::weak_ptr<a3d::Node> node;
            a3d::math::vec3          worldHitPosition;
            a3d::math::vec3          worldHitNormal;
        };

        /// Public Lifecycle Functions ///

        App(int argc, char* argv[]);
        ~App() override;

    protected:
        /// Application Protected Member Functions ///

        std::unique_ptr<a3d::Scene> init() override;
        a3d::SimulationConfig       simulationConfig() const override;
        bool                        shouldContinue(const a3d::Scene& scene) override;

        void hostUpdate(a3d::Runner& runner, a3d::Scene& scene, const a3d::Runner::UpdateInfo& info) override;

        void inputDidUpdate(a3d::Runner&                         runner,
                            a3d::Scene&                          scene,
                            a3d::InputContext&                   inputContext,
                            const a3d::InputContext::UpdateInfo& info) override;

        void sceneWillStep(a3d::Runner& runner, a3d::Scene& scene, const a3d::Scene::StepInfo& info) override;

        void frameDidBegin(a3d::Runner&                        runner,
                           a3d::Scene&                         scene,
                           a3d::VisualWorld&                   visualWorld,
                           const a3d::VisualWorld::RenderInfo& info) override;

    private:
        /// Private Member Functions ///

        void select(std::optional<PickResult> selection);
        void resetSimulation();

        /// Private Member Variables ///

        std::unique_ptr<a3d::Window>        _window;
        a3d::ext::TurntableCameraController _cameraController;
        std::shared_ptr<a3d::Node>          _cameraNode;
        std::shared_ptr<a3d::Node>          _simulationRoot;
        std::optional<PickResult>           _selection;
        // std::chrono::milliseconds           _pendingHiccup;
        bool                                _resetRequested;
    };

}

#endif // AVARA3D_DEMO_PHYSICSSANDBOX_APP_H
