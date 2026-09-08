//
//  App.h
//  janus
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_DEMO_JANUS_APP_H
#define AVARA3D_DEMO_JANUS_APP_H

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "a3d/Application.h"
#include "a3d/Math.h"
#include "a3d/extension/TransientsTracker.h"
#include "a3d/extension/camera/TurntableCameraController.h"

#include "TransientsBuilder.h"

namespace a3d {

class Node;
class Window;

} // namespace a3d

namespace demo::janus {

class App : public a3d::Application {

public:
    // [Public Types]

    struct PickResult {
        std::weak_ptr<a3d::Node> node;
        a3d::math::vec3          hitPosition;
    };

    // [Public Lifecycle Functions]

    App(int argc, char* argv[]);
    ~App() override;

protected:
    // [Application Protected Member Functions]

    std::unique_ptr<a3d::Scene> init() override;
    a3d::SimulationConfig       simulationConfig() const override;
    bool                        shouldContinue(const a3d::Scene& scene) override;

    void runnerUpdate(a3d::Runner& runner, a3d::Scene& scene, const a3d::Runner::UpdateInfo& info) override;

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
    // [Private Types]

    enum class Action {
        Drop,
        Throw,
        Poke
    };

    enum class DropAction {
        Rocks,
        Coins,
        Balls
    };

    enum class ThrowAction {
        Hammer,
        Hula,
        Duck
    };

    enum class Pokiness {
        Soft,
        Hard,
        Flip
    };

    struct PendingAction {
        Action                   action;
        PickResult               target;
        unsigned                 generation;
        std::weak_ptr<a3d::Node> dynamicsRoot;
        a3d::math::vec3          cameraPosition;
        a3d::math::vec3          rayDirection;
    };

    enum class PickPurpose : std::uint8_t {
        Hover = 1 << 0,
        Select = 1 << 1,
        Target = 1 << 2,
        All = UINT8_MAX
    };

    struct PickIgnore {
        std::weak_ptr<a3d::Node> node;
        PickPurpose              purposes {PickPurpose::All};
        std::optional<double>    remainingTime;
    };

    struct NodeTransform {
        a3d::Node*      node;
        a3d::math::mat4 transform;
    };

    // [Private Member Functions]

    bool drawPanel();
    bool drawDemoPanel();
    bool drawDevPanel();
    void hover(const a3d::VisualWorld& visualWorld, const a3d::math::vec2& screenPosition);
    void hover(const std::shared_ptr<a3d::Node>& node);
    void select(const a3d::VisualWorld& visualWorld, const a3d::math::vec2& screenPosition);
    void select(std::optional<PickResult> pickResult);
    std::optional<PickResult>     target(const a3d::Scene& scene, const a3d::math::vec2& screenPosition) const;
    void                          queueAction(const a3d::math::vec2& screenPosition);
    void                          performAction(const PendingAction& action);
    std::vector<const a3d::Node*> pickIgnoredNodes(PickPurpose purpose) const;
    void                          saveDynamicsTransforms();
    void                          restoreDynamics() const;
    void                          reset();

    // [Private Member Variables]

    std::unique_ptr<a3d::Window>        _window;
    std::shared_ptr<a3d::Node>          _dynamicsRoot;
    std::shared_ptr<a3d::Node>          _transientsRoot;
    std::shared_ptr<a3d::Node>          _cameraNode;
    a3d::ext::TurntableCameraController _cameraController;
    std::weak_ptr<a3d::Node>            _hoveredNode;
    std::optional<PickResult>           _selection;
    Action                              _action;
    DropAction                          _dropAction;
    ThrowAction                         _throwAction;
    Pokiness                            _pokiness;
    TransientsBuilder                   _transientsBuilder;
    a3d::ext::TransientsTracker         _transientsTracker;
    double                              _backgroundRotationTime;
    std::vector<PickIgnore>             _pickIgnores;
    std::vector<NodeTransform>          _dynamicsTransforms;
    unsigned                            _simulationGeneration;
    bool                                _pendingReset;
};

} // namespace demo::janus

#endif // AVARA3D_DEMO_JANUS_APP_H
