//
//  App.h
//  janus
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_DEMO_JANUS_APP_H
#define AVARA3D_DEMO_JANUS_APP_H

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "a3d/Application.h"
#include "a3d/Math.h"
#include "a3d/extension/TransientNodeRegistry.h"
#include "a3d/extension/Wander.h"
#include "a3d/extension/camera/TurntableCameraController.h"

namespace a3d {

    class Node;
    class Window;

}

namespace demo::janus {

    class App : public a3d::Application {

    public:
        /// Public Types ///

        struct PickResult {
            std::weak_ptr<a3d::Node> node;
            a3d::math::vec3          worldHitPosition;
            a3d::math::vec3          worldHitNormal;
        };

        struct ImpactInfo {
            std::string nodes;
            float       impulse {0.0f};
            float       penetration {0.0f};
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
        void sceneDidStep(a3d::Runner& runner, a3d::Scene& scene, const a3d::Scene::StepInfo& info) override;

        void frameDidBegin(a3d::Runner&                        runner,
                           a3d::Scene&                         scene,
                           a3d::VisualWorld&                   visualWorld,
                           const a3d::VisualWorld::RenderInfo& info) override;

        // void contactDidBegin(a3d::Runner&               runner,
        //                      a3d::Scene&                scene,
        //                      a3d::PhysicsWorld&         physicsWorld,
        //                      const a3d::PhysicsContact& contact) override;
        // void contactDidContinue(a3d::Runner&               runner,
        //                         a3d::Scene&                scene,
        //                         a3d::PhysicsWorld&         physicsWorld,
        //                         const a3d::PhysicsContact& contact) override;
        // void contactDidEnd(a3d::Runner&               runner,
        //                    a3d::Scene&                scene,
        //                    a3d::PhysicsWorld&         physicsWorld,
        //                    const a3d::PhysicsContact& contact) override;

    private:
        /// Private Types ///

        enum class Action {
            Impulse,
            Shoot,
            Drop
        };

        /// Private Member Functions ///

        void select(std::optional<PickResult> selection);
        void useAction(a3d::Scene& scene, const a3d::math::vec2& screenPosition);
        void resetSimulation();

        /// Private Member Variables ///

        std::unique_ptr<a3d::Window>                   _window;
        std::shared_ptr<a3d::Node>                     _simulationRoot;
        std::shared_ptr<a3d::Node>                     _cameraNode;
        a3d::ext::TurntableCameraController            _cameraController;
        std::optional<PickResult>                      _selection;
        std::shared_ptr<a3d::Node>                     _cursorMarker;
        std::optional<PickResult>                      _actionTarget;
        Action                                         _action;
        a3d::ext::TransientNodeRegistry                _transients;
        double                                         _backgroundRotationTime;
        std::vector<std::unique_ptr<a3d::ext::Wander>> _orbWanders;
        // std::optional<ImpactInfo>                      _lastImpact;
        // float                                          _peakImpactImpulse;
        bool _resetRequested;
    };

}

#endif // AVARA3D_DEMO_JANUS_APP_H
