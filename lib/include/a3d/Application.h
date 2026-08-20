//
//  Application.h
//  avara3d
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_APPLICATION_H
#define AVARA3D_APPLICATION_H

#include <functional>
#include <memory>
#include <vector>

#include "a3d/Runner.h"
#include "a3d/SimulationConfig.h"
#include "a3d/input/InputContext.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/physics/PhysicsContact.h"
#include "a3d/profile/Timer.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"
#include "log/Log.h"

// #include "a3d/TestAccessFwd.h"

namespace a3d {

    /**
     * @brief Coordinates the top-level lifecycle of an A3D application.
     *
     * Application owns the Scene and Runner created for an application and
     * coordinates initialization, host updates, simulation callbacks, rendering,
     * and shutdown. Run() takes ownership of an Application instance and drives
     * its platform-specific host loop.
     */
    class Application {

    public:
        // [Public Static Member Functions]

        /**
         * @brief Runs @p application using the platform host loop.
         *
         * Run() takes ownership of @p application. On native builds it blocks
         * until execution stops and shutdown completes. On web builds it installs
         * the browser main loop and returns after scheduling execution; the
         * Application is destroyed when that loop terminates.
         *
         * @return zero after successful launch or completion.
         * @throws std::invalid_argument if @p application is nullptr.
         */
        static int Run(std::unique_ptr<Application> application);

        // [Public Lifecycle Functions]

        /**
         * @brief Creates application state from process arguments and configures logging.
         *
         * @param argc number of process command-line arguments.
         * @param argv process command-line argument array.
         * @param logLevel initial application log level.
         */
        Application(int argc, char* argv[], Log::Level logLevel = Log::Level::Info);

        Application(const Application&)            = delete;
        Application& operator=(const Application&) = delete;

        Application(Application&&)            = delete;
        Application& operator=(Application&&) = delete;

        virtual ~Application();

    protected:
        // [Protected Types]

        using SceneCommand = std::function<void(Scene&)>;

        // [Protected Member Functions]

        virtual std::unique_ptr<Scene>  init() = 0;
        virtual SimulationConfig        simulationConfig() const;
        virtual bool                    shouldContinue(const Scene& scene);
        virtual void                    didShutdown();

        Runner&                         runner();
        const Runner&                   runner() const;

        Scene&                          scene();
        const Scene&                    scene() const;

        void                            queueScenePreStepCommand(SceneCommand command);
        void                            queueScenePostStepCommand(SceneCommand command);

        const std::vector<std::string>& args() const;

        virtual void runnerUpdate(Runner& runner, Scene& scene, const Runner::UpdateInfo& info);

        virtual void inputDidUpdate(Runner&       runner,
                                    Scene&        scene,
                                    InputContext& inputContext,
                                    const InputContext::UpdateInfo&);

        virtual void sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info);
        virtual void sceneDidStep(Runner& runner, Scene& scene, const Scene::StepInfo& info);

        virtual void frameDidBegin(Runner&                        runner,
                                   Scene&                         scene,
                                   VisualWorld&                   visualWorld,
                                   const VisualWorld::RenderInfo& info);

        virtual void contactDidBegin(Runner&               runner,
                                     Scene&                scene,
                                     PhysicsWorld&         physicsWorld,
                                     const PhysicsContact& contact);
        virtual void contactDidContinue(Runner&               runner,
                                        Scene&                scene,
                                        PhysicsWorld&         physicsWorld,
                                        const PhysicsContact& contact);
        virtual void contactDidEnd(Runner&               runner,
                                   Scene&                scene,
                                   PhysicsWorld&         physicsWorld,
                                   const PhysicsContact& contact);

    private:
        // [Private Member Functions]

        void initLog(Log::Level level);
        void prepare();
        bool update();
        void shutdown() noexcept;
        void registerCallbacks();

        void executeSceneCommands(std::vector<SceneCommand>& queue, Scene& scene);

        void dispatchRunnerUpdate(Runner& runner, const Runner::UpdateInfo& info);
        void dispatchInputContextDidUpdate(InputContext& inputContext, const InputContext::UpdateInfo& info);
        void dispatchSceneWillStep(Scene& scene, const Scene::StepInfo& info);
        void dispatchSceneDidStep(Scene& scene, const Scene::StepInfo& info);
        void dispatchDidBeginFrame(VisualWorld& visualWorld, const VisualWorld::RenderInfo& info);
        void dispatchContactDidBegin(PhysicsWorld& physicsWorld, const PhysicsContact& contact);
        void dispatchContactDidContinue(PhysicsWorld& physicsWorld, const PhysicsContact& contact);
        void dispatchContactDidEnd(PhysicsWorld& physicsWorld, const PhysicsContact& contact);

        // [Private Member Variables]

        std::vector<std::string>  _args;
        std::unique_ptr<Scene>    _scene;
        std::unique_ptr<Runner>   _runner; // Runner must be destroyed before Scene
        std::vector<SceneCommand> _scenePreStepQueue;
        std::vector<SceneCommand> _scenePostStepQueue;
        bool                      _didShutdown;
        Timer                     _startupTimer;

        // [Test Access]

        //friend class testing::ApplicationTestAccess;
    };

}

#endif //AVARA3D_APPLICATION_H
