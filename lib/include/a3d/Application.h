//
//  Application.h
//  avara3d
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_APPLICATION_H
#define AVARA3D_APPLICATION_H

#include <memory>
#include <vector>

#include "a3d/Runner.h"
#include "a3d/SimulationConfig.h"
#include "a3d/input/InputContext.h"
#include "a3d/profile/Timer.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"
#include "log/Log.h"

#include "a3d/TestAccessFwd.h"

namespace a3d {

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
        /// Protected Member Functions ///

        virtual std::unique_ptr<Scene>  init() = 0;
        virtual SimulationConfig        simulationConfig() const;
        virtual bool                    shouldContinue(const Scene& scene);
        virtual void                    didShutdown();

        Runner&                         runner();
        const Runner&                   runner() const;

        Scene&                          scene();
        const Scene&                    scene() const;

        const std::vector<std::string>& args() const;

        /// Runner Callbacks ///

        // general once-per-host-cycle callback, independent of InputContext
        // presence, for non-input host orchestration before simulation scheduling.
        virtual void                    hostUpdate(Runner& runner, const Runner::UpdateInfo& info);

        /// InputContext Callbacks ///

        // called once after the attached InputContext updates and before
        // hostUpdate() and simulation scheduling. skipped when no InputContext
        // is attached; intended for input-driven host, view, and Runner behavior.
        virtual void inputContextDidUpdate(InputContext& inputContext, const InputContext::UpdateInfo& info);

        /// Scene Callbacks ///

        virtual void sceneWillStep(Scene& scene, const Scene::StepInfo& info);
        virtual void sceneDidStep(Scene& scene, const Scene::StepInfo& info);

        /// VisualWorld Callbacks ///

        virtual void didBeginFrame(VisualWorld& visualWorld, const VisualWorld::RenderInfo& info);

    private:
        /// Private Member Functions ///

        void initLog(Log::Level level);
        void prepare();
        bool update();
        void shutdown() noexcept;
        void registerCallbacks();

        void dispatchInputContextDidUpdate(InputContext& inputContext, const InputContext::UpdateInfo& info);
        void dispatchHostUpdate(Runner& runner, const Runner::UpdateInfo& info);
        void dispatchSceneWillStep(Scene& scene, const Scene::StepInfo& info);
        void dispatchSceneDidStep(Scene& scene, const Scene::StepInfo& info);
        void dispatchDidBeginFrame(VisualWorld& visualWorld, const VisualWorld::RenderInfo& info);

        /// Private Member Variables ///

        std::vector<std::string> _args;
        std::unique_ptr<Scene>   _scene;
        std::unique_ptr<Runner>  _runner; // Runner must be destroyed before Scene
        bool                     _didShutdown;
        Timer                    _startupTimer;

        /// Test Access ///

        friend class testing::ApplicationTestAccess;
    };
}

#endif //AVARA3D_APPLICATION_H
