//
//  Application.h
//  avara3d
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_APPLICATION_H
#define AVARA3D_APPLICATION_H

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include "a3d/CommandQueue.h"
#include "a3d/Runner.h"
#include "a3d/SimulationConfig.h"
#include "a3d/profile/Timer.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"
#include "log/Log.h"

#include "a3d/TestAccessFwd.h"

namespace a3d {

    class InputContext;

    class Application {

    public:
        /// Public Static Member Functions ///

        static int Run(std::unique_ptr<Application> application);

        /// Public Lifecycle Functions ///

        Application(int argc, char* argv[], Log::Level logLevel = Log::Level::Info);

        Application(const Application&)            = delete;
        Application& operator=(const Application&) = delete;

        Application(Application&&)            = delete;
        Application& operator=(Application&&) = delete;

        virtual ~Application();

    protected:
        /// Protected Types ///

        // using SceneCommand  = CommandQueue<Scene>::value_type;
        // using RenderCommand = CommandQueue<VisualWorld>::value_type;

        /// Protected Member Functions ///

        virtual std::unique_ptr<Scene>  init() = 0;
        virtual SimulationConfig        simulationConfig() const;
        virtual bool                    shouldContinue(const Scene& scene);
        virtual void                    didShutdown();

        // void                            queueSceneCommand(SceneCommand command);
        // void                            queueRenderCommand(RenderCommand command);

        Runner&                         runner();
        const Runner&                   runner() const;

        const std::vector<std::string>& args() const;

        /// InputContext Callbacks ///

        // Called once after the attached InputContext updates and before
        // hostUpdate() and simulation scheduling. Skipped when no InputContext
        // is attached; intended for input-driven host, view, and Runner behavior.
        virtual void                    inputContextDidUpdate(Runner&                   runner,
                                                              InputContext&             inputContext,
                                                              const Runner::UpdateInfo& info);

        /// Runner Callbacks ///

        // General once-per-host-cycle callback, independent of InputContext
        // presence, for non-input host orchestration before simulation scheduling.
        virtual void                    hostUpdate(Runner& runner, const Runner::UpdateInfo& info);

        /// Scene Callbacks ///

        virtual void                    sceneWillStep(Scene& scene, const Scene::StepInfo& info);
        virtual void                    sceneDidStep(Scene& scene, const Scene::StepInfo& info);

        /// VisualWorld Callbacks ///

        // virtual void					renderFrame(VisualWorld& visualWorld,
        // 					                        const VisualWorld::RenderInfo& info);
        virtual void didBeginFrame(VisualWorld& visualWorld, const VisualWorld::RenderInfo& info);

    private:
        /// Private Member Functions ///

        // template<typename Context>
        // static void executePendingCommands(CommandQueue<Context>& queue, Context& context);

        void        initLog(Log::Level level);
        void        prepare();
        bool        update();
        void        shutdown() noexcept;
        void        registerCallbacks();

        void        dispatchUpdate(Runner& runner, const Runner::UpdateInfo& info);
        void        dispatchSceneWillStep(Scene& scene, const Scene::StepInfo& info);
        void        dispatchSceneDidStep(Scene& scene, const Scene::StepInfo& info);
        void        dispatchDidBeginFrame(VisualWorld& visualWorld, const VisualWorld::RenderInfo& info);

        /// Private Member Variables ///

        std::vector<std::string>  _args;
        std::unique_ptr<Scene>    _scene;
        std::unique_ptr<Runner>   _runner; // Runner must be destroyed before Scene
        bool                      _didShutdown;
        // CommandQueue<Scene>       _sceneCommandQueue;
        // CommandQueue<VisualWorld> _renderCommandQueue;
        Timer                     _startupTimer;

        /// Test Access ///

        friend class testing::ApplicationTestAccess;
    };

    // template<typename Context>
    // void Application::executePendingCommands(CommandQueue<Context>& queue, Context& context) {
    //     const auto pendingCount = queue.size();
    //     for (std::size_t i = 0; i < pendingCount; ++i) {
    //         auto command = std::move(queue.front());
    //         queue.pop();
    //         command(context);
    //     }
    // }

}

#endif //AVARA3D_APPLICATION_H
