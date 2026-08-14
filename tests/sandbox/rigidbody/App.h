//
//  App.h
//  rigidbody
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TEST_RIGIDBODY_APP_H
#define AVARA3D_TEST_RIGIDBODY_APP_H

#include <memory>
#include <tuple>
#include <vector>

#include "a3d/Application.h"
#include "a3d/extension/camera/FlyCameraController.h"
#include "a3d/util/PeriodicTrigger.h"

namespace a3d {

    class Mesh;
    class Node;
    class PhysicsShape;
    class Scene;
    class Window;

}

namespace a3d::ext {

    struct WanderRotator;

}

namespace test::rigidbody {

    class App : public a3d::Application {

    public:
        /// Public Lifecycle Functions ///

        App(int argc, char* argv[]);
        ~App() override;

        /// Internal Types ///

        using DuckFruitDef = std::tuple<std::shared_ptr<a3d::Mesh>, std::shared_ptr<a3d::PhysicsShape>, float>;

    protected:
        /// Application Protected Member Functions ///

        std::unique_ptr<a3d::Scene> init() override;
        a3d::SimulationConfig       simulationConfig() const override;
        bool                        shouldContinue(const a3d::Scene& scene) override;

        void inputDidUpdate(a3d::Runner&                         runner,
                            a3d::Scene&                          scene,
                            a3d::InputContext&                   inputContext,
                            const a3d::InputContext::UpdateInfo& info) override;

        void sceneWillStep(a3d::Runner& runner, a3d::Scene& scene, const a3d::Scene::StepInfo& info) override;

    private:
        /// Private Member Variables ///

        std::unique_ptr<a3d::Window>             _window;
        a3d::ext::FlyCameraController            _cameraController;
        std::weak_ptr<a3d::Node>                 _duckNode;
        std::vector<DuckFruitDef>                _duckFruit;
        std::unique_ptr<a3d::ext::WanderRotator> _duckRotator;
        a3d::util::PeriodicTrigger               _duckFruitTrigger;
        a3d::util::PeriodicTrigger               _slurmTrigger;
        uint64_t                                 _contactBegins;
        uint64_t                                 _contactContinues;
        uint64_t                                 _contactEnds;
        std::weak_ptr<a3d::Node>                 _contactTestA;
        std::weak_ptr<a3d::Node>                 _contactTestB;
        std::weak_ptr<a3d::Node>                 _contactTestC;
        std::weak_ptr<a3d::Node>                 _contactTestD;
    };

}

#endif // AVARA3D_TEST_RIGIDBODY_APP_H
