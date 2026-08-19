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
#include "a3d/extension/Transients.h"
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
            a3d::math::vec3          hitPosition;
            a3d::math::vec3          hitNormal;
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

    private:
        /// Private Types ///

        enum class Action {
            Drop,
            Throw,
            Poke
        };

        enum class DropAction {
            Blocks,
            Balls
        };

        enum class ThrowAction {
            Ring,
            Duck
        };

        enum class Pokiness {
            Soft,
            Hard
        };

        struct PendingAction {
            Action                   action;
            PickResult               target;
            std::weak_ptr<a3d::Node> simulationRoot;
            a3d::math::vec3          cameraPosition;
            a3d::math::vec3          rayDirection;
        };

        struct PickIgnore {
            std::weak_ptr<a3d::Node> node;
            double                   remainingTime;
        };

        /// Private Member Functions ///

        bool drawPanel();
        void hover(std::shared_ptr<a3d::Node> node);
        void select(std::optional<PickResult> pickResult);
        void queueAction(const a3d::math::vec2& screenPosition);
        void performAction(const PendingAction& action);
        void reset();

        /// Private Member Variables ///

        std::unique_ptr<a3d::Window>        _window;
        std::shared_ptr<a3d::Node>          _simulationRoot;
        std::shared_ptr<a3d::Node>          _cameraNode;
        a3d::ext::TurntableCameraController _cameraController;
        std::weak_ptr<a3d::Node>            _hoveredNode;
        std::optional<PickResult>           _selection;
        std::shared_ptr<a3d::Node>          _cursorMarker;
        std::optional<PickResult>           _actionTarget;
        Action                              _action;
        DropAction                          _dropAction;
        ThrowAction                         _throwAction;
        Pokiness                            _pokiness;
        a3d::ext::Transients                _transients;
        double                              _backgroundRotationTime;
        std::vector<a3d::ext::Wander>       _orbWanders;
        std::vector<PickIgnore>             _pickIgnores;
        bool                                _pendingReset;
    };

}

#endif // AVARA3D_DEMO_JANUS_APP_H
