//
//  RunnerExecutionTests.cc
//  avara3d
//
//  Created by Morgan Davis on 7/27/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include "a3d/Application.h"
#include "a3d/Runner.h"
#include "a3d/SimulationConfig.h"
#include "a3d/input/InputContext.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsInventory.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/physics/shape/primitive/SpherePhysicsShape.h"
#include "a3d/profile/FrameStats.h"
#include "a3d/profile/FrameStatsHistory.h"
#include "a3d/profile/Profiler.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

namespace a3d::testing {

    class ApplicationTestAccess {

    public:
        static void prepare(Application& application) {
            application.prepare();
        }

        static void shutdown(Application& application) {
            application.shutdown();
        }

        static Runner& runner(Application& application) {
            return *application._runner;
        }

        static std::size_t sceneCommandCount(const Application& application) {

            return application._sceneCommandQueue.size();
        }

        static std::size_t renderCommandCount(const Application& application) {

            return application._renderCommandQueue.size();
        }

        static void dispatchSceneWillStep(Application& application, Scene& scene, const Scene::StepInfo& info) {

            application.dispatchSceneWillStep(scene, info);
        }

        static void dispatchSceneDidStep(Application& application, Scene& scene, const Scene::StepInfo& info) {

            application.dispatchSceneDidStep(scene, info);
        }

        static void dispatchPendingRenderCommands(Application&                   application,
                                                  VisualWorld&                   visualWorld,
                                                  const VisualWorld::RenderInfo& info) {

            application.dispatchPendingRenderCommands(visualWorld, info);
        }

        static void dispatchRenderFrame(Application&                   application,
                                        VisualWorld&                   visualWorld,
                                        const VisualWorld::RenderInfo& info) {

            application.dispatchRenderFrame(visualWorld, info);
        }
    };

    class RunnerTestAccess {

    public:
        using TimePoint = Runner::Clock::time_point;

        static void start(Runner& runner, TimePoint now) {
            runner.start(now);
        }

        static bool update(Runner& runner, TimePoint now) {
            return runner.update(now);
        }

        static TimePoint startTime(const Runner& runner) {
            return runner._startTime;
        }

        static const Runner::UpdateInfo& updateInfo(const Runner& runner) {
            return runner._updateInfo;
        }

        static const FrameStatsHistory& frameStatsHistory(const Runner& runner) {
            return runner._frameStatsHistory;
        }

        static std::uint64_t completedRenderFrameCount(const Runner& runner) {
            return runner._renderedFrameCount;
        }

        static double simulationAccumulator(const Runner& runner) {
            return runner._simulationTimeAccumulator;
        }
    };

}

namespace {

    using TimePoint = a3d::testing::RunnerTestAccess::TimePoint;

    struct CallbackTime {
        double elapsedTime;
        double deltaTime;
    };

    struct RecordedStep {
        std::uint64_t stepIndex;
        double        startTime;
        double        endTime;
        double        deltaTime;
    };

    class RecordingInputContext final : public a3d::InputContext {

    public:
        explicit RecordingInputContext(std::vector<std::string_view>& stages):
            _stages {stages},
            _updateCount {} {}

        void update() override {
            _stages.push_back("input");
            ++_updateCount;
        }

        void        attachedToScene(a3d::Scene&) override {}

        void        visualWorldAttachedToScene(a3d::Scene&) override {}

        std::size_t updateCount() const {
            return _updateCount;
        }

        void reset() {
            _updateCount = 0;
        }

    private:
        std::vector<std::string_view>& _stages;
        std::size_t                    _updateCount;
    };

    class PassiveInputContext final : public a3d::InputContext {

    public:
        void update() override {}

        void attachedToScene(a3d::Scene&) override {}

        void visualWorldAttachedToScene(a3d::Scene&) override {}
    };

    class AdapterRenderContext final : public a3d::RenderContext {

    public:
        AdapterRenderContext():
            RenderContext(RenderingApi::OpenGL) {

            // This fixture exercises Application's callback adapters without
            // initializing or destroying an OpenGL backend.
            (void) _renderer.release();
        }

        bool vSyncEnabled() const override {
            return false;
        }

        void             vSyncEnabled(bool) override {}

        void             beginFrame(const a3d::Scene&) override {}

        void             endFrame(const a3d::Scene&) override {}

        void             swapBuffers() override {}

        a3d::math::uvec2 viewportLogicalSize() const override {
            return {1, 1};
        }

        a3d::math::uvec2 framebufferSize() const override {
            return {1, 1};
        }

        unsigned defaultFramebuffer() const override {
            return 0;
        }
    };

    class RecordingApplication final : public a3d::Application {

    public:
        using HostAction       = std::function<void(a3d::Runner&, const a3d::Runner::UpdateInfo&)>;
        using SimulationAction = std::function<void(a3d::Scene&, const a3d::Scene::StepInfo&)>;
        using RenderAction     = std::function<void(a3d::VisualWorld&, const a3d::VisualWorld::RenderInfo&)>;

        RecordingApplication():
            Application(1, Arguments()),
            hostAction {},
            simulationWillAction {},
            simulationDidAction {},
            renderAction {} {}

        void enqueueScene(SceneCommand command) {
            queueSceneCommand(std::move(command));
        }

        void enqueueRender(RenderCommand command) {
            queueRenderCommand(std::move(command));
        }

        void invokeHostUpdate(a3d::Runner& runner, const a3d::Runner::UpdateInfo& info) {

            hostUpdate(runner, info);
        }

        HostAction       hostAction;
        SimulationAction simulationWillAction;
        SimulationAction simulationDidAction;
        RenderAction     renderAction;

    protected:
        std::unique_ptr<a3d::Scene> init() override {
            return std::make_unique<a3d::Scene>();
        }

        /// Runner Callbacks ///

        void hostUpdate(a3d::Runner& runner, const a3d::Runner::UpdateInfo& info) override {

            if (hostAction) {
                hostAction(runner, info);
            }
        }

        /// Scene Callbacks ///

        void sceneWillStep(a3d::Scene& scene, const a3d::Scene::StepInfo& info) override {

            if (simulationWillAction) {
                simulationWillAction(scene, info);
            }
        }

        void sceneDidStep(a3d::Scene& scene, const a3d::Scene::StepInfo& info) override {

            if (simulationDidAction) {
                simulationDidAction(scene, info);
            }
        }

        /// VisualWorld Callbacks ///

        void renderFrame(a3d::VisualWorld& visualWorld, const a3d::VisualWorld::RenderInfo& info) override {

            if (renderAction) {
                renderAction(visualWorld, info);
            }
        }

    private:
        static char** Arguments() {

            static char  executableName[] = "a3d-runner-execution-tests";
            static char* arguments[]      = {executableName};

            return arguments;
        }
    };

    struct ApplicationRunnerAccessResult {
        std::size_t hostUpdateCount;
        bool        constAndMutableAccessMatch;
        bool        callbackAndAccessMatch;
        double      initialTimeScale;
        double      hostUpdateTimeScale;
    };

    class RunnerAccessApplication final : public a3d::Application {

    public:
        explicit RunnerAccessApplication(ApplicationRunnerAccessResult& result):
            Application(1, Arguments()),
            _result {result} {}

        a3d::Runner& runnerBeforeInitialization() {
            return runner();
        }

    protected:
        std::unique_ptr<a3d::Scene> init() override {

            auto scene = std::make_unique<a3d::Scene>();
            scene->inputContext(std::make_unique<PassiveInputContext>());
            return scene;
        }

        /// Runner Callbacks ///

        void hostUpdate(a3d::Runner& callbackRunner, const a3d::Runner::UpdateInfo&) override {

            ++_result.hostUpdateCount;

            const auto& constApplication       = *this;
            _result.constAndMutableAccessMatch = &constApplication.runner() == &runner();
            _result.callbackAndAccessMatch     = &callbackRunner == &runner();
            _result.initialTimeScale           = constApplication.runner().timeScale();

            callbackRunner.timeScale(0.5);
            _result.hostUpdateTimeScale = callbackRunner.timeScale();
            callbackRunner.stop();
        }

    private:
        static char** Arguments() {

            static char  executableName[] = "a3d-runner-execution-tests";
            static char* arguments[]      = {executableName};

            return arguments;
        }

        ApplicationRunnerAccessResult& _result;
    };

    struct StopDuringHostResult {
        std::size_t hostUpdateCount;
        std::size_t simulationWillCount;
        std::size_t simulationDidCount;
        bool        hostObservedStoppedState;
    };

    class StopDuringHostApplication final : public a3d::Application {

    public:
        explicit StopDuringHostApplication(StopDuringHostResult& result):
            Application(1, Arguments()),
            _result {result},
            _stopDuringHost {false} {}

        void stopDuringNextHostUpdate() {
            _stopDuringHost = true;
        }

    protected:
        std::unique_ptr<a3d::Scene> init() override {

            auto scene = std::make_unique<a3d::Scene>();
            scene->inputContext(std::make_unique<PassiveInputContext>());
            scene->physicsWorld(std::make_unique<a3d::PhysicsWorld>());
            return scene;
        }

        a3d::SimulationConfig simulationConfig() const override {

            auto config     = a3d::SimulationConfig {};
            config.timeStep = 0.125;
            return config;
        }

        /// Runner Callbacks ///

        void hostUpdate(a3d::Runner& callbackRunner, const a3d::Runner::UpdateInfo&) override {

            ++_result.hostUpdateCount;

            if (_stopDuringHost) {
                callbackRunner.stop();
                _result.hostObservedStoppedState = callbackRunner.state() == a3d::Runner::State::Stopped;
            }
        }

        /// Scene Callbacks ///

        void sceneWillStep(a3d::Scene&, const a3d::Scene::StepInfo&) override {
            ++_result.simulationWillCount;
        }

        void sceneDidStep(a3d::Scene&, const a3d::Scene::StepInfo&) override {
            ++_result.simulationDidCount;
        }

    private:
        static char** Arguments() {

            static char  executableName[] = "a3d-runner-execution-tests";
            static char* arguments[]      = {executableName};

            return arguments;
        }

        StopDuringHostResult& _result;
        bool                  _stopDuringHost;
    };

    TimePoint AtMilliseconds(std::int64_t milliseconds) {
        return TimePoint {} + std::chrono::milliseconds(milliseconds);
    }

    TimePoint AtMicroseconds(std::int64_t microseconds) {
        return TimePoint {} + std::chrono::microseconds(microseconds);
    }

    void Expect(bool condition, std::string_view message) {

        if (!condition) {
            throw std::runtime_error(std::string(message));
        }
    }

    void ExpectNear(double actual, double expected, std::string_view message, double tolerance = 1e-12) {

        if (std::abs(actual - expected) > tolerance) {
            throw std::runtime_error(std::string(message) + ": expected " + std::to_string(expected) + ", got "
                                     + std::to_string(actual));
        }
    }

    template<typename Function>
    void ExpectInvalidArgument(Function&& function, std::string_view message) {

        bool rejected = false;
        try {
            function();
        }
        catch (const std::invalid_argument&) {
            rejected = true;
        }

        Expect(rejected, message);
    }

    template<typename Function>
    void ExpectLogicError(Function&& function, std::string_view message) {

        bool rejected = false;
        try {
            function();
        }
        catch (const std::logic_error&) {
            rejected = true;
        }

        Expect(rejected, message);
    }

    a3d::SimulationConfig MakeSimulationConfig(double        deltaTime       = 0.125,
                                               std::uint32_t maxCatchUpSteps = 8,
                                               double        timeScale       = 1.0) {

        auto configuration            = a3d::SimulationConfig {};
        configuration.timeStep        = deltaTime;
        configuration.maxCatchUpSteps = maxCatchUpSteps;
        configuration.timeScale       = timeScale;
        return configuration;
    }

    RecordedStep RecordStep(const a3d::Scene::StepInfo& info) {
        return {.stepIndex = info.stepIndex,
                .startTime = info.startTime,
                .endTime   = info.endTime,
                .deltaTime = info.deltaTime};
    }

    void ExpectStep(const RecordedStep& step,
                    std::uint64_t       stepIndex,
                    double              startTime,
                    double              endTime,
                    double              deltaTime,
                    std::string_view    message) {

        Expect(step.stepIndex == stepIndex, std::string(message) + " step index");
        ExpectNear(step.startTime, startTime, std::string(message) + " start time");
        ExpectNear(step.endTime, endTime, std::string(message) + " end time");
        ExpectNear(step.deltaTime, deltaTime, std::string(message) + " delta time");
    }

    const a3d::FrameStats& LatestFrameStats(const a3d::Runner& runner) {

        const auto& samples = a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples();

        Expect(!samples.empty(), "the Runner should have a statistics sample");
        return std::get<1>(samples.back());
    }

    std::shared_ptr<a3d::Node> AddMovingDynamicBody(a3d::Scene& scene, double velocity = 1.0) {

        auto node = std::make_shared<a3d::Node>("moving dynamic body");
        node->physicsBody(std::make_unique<a3d::PhysicsBody>(a3d::PhysicsBody::Type::Dynamic,
                                                             std::make_shared<a3d::SpherePhysicsShape>(1.0f)));
        scene.rootNode()->addChild(node);

        auto body = node->physicsBody();
        body->affectedByGravity(false);
        body->allowsResting(false);
        body->linearVelocity({static_cast<float>(velocity), 0.0f, 0.0f});

        return node;
    }

    void FirstUpdateHasZeroDelta() {

        a3d::Scene                scene;
        a3d::Runner               runner(scene);
        std::vector<CallbackTime> times;
        runner.updateCallback([&times](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
            times.push_back({info.elapsedTime, info.deltaTime});
        });

        runner.start();

        Expect(runner.update(), "the running Runner should continue");
        Expect(times.size() == 1, "the Runner callback should run once");
        Expect(times[0].elapsedTime >= 0.0, "the first elapsed time should use the monotonic update clock");
        ExpectNear(times[0].deltaTime, 0.0, "the first update delta should be zero");
    }

    void ElapsedTimeProgresses() {

        a3d::Scene                scene;
        a3d::Runner               runner(scene);
        std::vector<CallbackTime> times;
        runner.updateCallback([&times](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
            times.push_back({info.elapsedTime, info.deltaTime});
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(2000));
        a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(2100));
        a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(2350));
        a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(2900));

        Expect(times.size() == 3, "the Runner callback should run three times");
        ExpectNear(times[0].elapsedTime, 0.1, "first elapsed time");
        ExpectNear(times[1].elapsedTime, 0.35, "second elapsed time");
        ExpectNear(times[2].elapsedTime, 0.9, "third elapsed time");
    }

    void DeltaTimeProgresses() {

        a3d::Scene                scene;
        a3d::Runner               runner(scene);
        std::vector<CallbackTime> times;
        runner.updateCallback([&times](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
            times.push_back({info.elapsedTime, info.deltaTime});
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(3000));
        a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(3100));
        a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(3350));
        a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(3900));

        Expect(times.size() == 3, "the Scene callback should run three times");
        ExpectNear(times[0].deltaTime, 0.0, "first delta time");
        ExpectNear(times[1].deltaTime, 0.25, "second delta time");
        ExpectNear(times[2].deltaTime, 0.55, "third delta time");
    }

    void UpdateIndexProgresses() {

        a3d::Scene  scene;
        a3d::Runner runner(scene);
        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(4000));

        a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(4100));
        Expect(a3d::testing::RunnerTestAccess::updateInfo(runner).updateIndex == 0,
               "the first update index should be zero");

        a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(4200));
        Expect(a3d::testing::RunnerTestAccess::updateInfo(runner).updateIndex == 1,
               "the second update index should be one");

        a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(4300));
        Expect(a3d::testing::RunnerTestAccess::updateInfo(runner).updateIndex == 2,
               "the third update index should be two");
    }

    void RunnerClocksAreIndependent() {

        a3d::Scene                firstScene;
        std::vector<CallbackTime> firstTimes;
        a3d::Scene                secondScene;
        std::vector<CallbackTime> secondTimes;
        a3d::Runner               firstRunner(firstScene);
        a3d::Runner               secondRunner(secondScene);
        firstRunner.updateCallback([&firstTimes](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
            firstTimes.push_back({info.elapsedTime, info.deltaTime});
        });
        secondRunner.updateCallback([&secondTimes](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
            secondTimes.push_back({info.elapsedTime, info.deltaTime});
        });

        a3d::testing::RunnerTestAccess::start(firstRunner, AtMilliseconds(10000));
        a3d::testing::RunnerTestAccess::start(secondRunner, AtMilliseconds(50000));

        a3d::testing::RunnerTestAccess::update(firstRunner, AtMilliseconds(10100));
        a3d::testing::RunnerTestAccess::update(secondRunner, AtMilliseconds(50700));
        a3d::testing::RunnerTestAccess::update(firstRunner, AtMilliseconds(10350));
        a3d::testing::RunnerTestAccess::update(secondRunner, AtMilliseconds(51600));

        Expect(firstTimes.size() == 2, "the first callback should run twice");
        Expect(secondTimes.size() == 2, "the second callback should run twice");
        ExpectNear(firstTimes[0].elapsedTime, 0.1, "first Runner first elapsed time");
        ExpectNear(firstTimes[0].deltaTime, 0.0, "first Runner first delta time");
        ExpectNear(firstTimes[1].elapsedTime, 0.35, "first Runner second elapsed time");
        ExpectNear(firstTimes[1].deltaTime, 0.25, "first Runner second delta time");
        ExpectNear(secondTimes[0].elapsedTime, 0.7, "second Runner first elapsed time");
        ExpectNear(secondTimes[0].deltaTime, 0.0, "second Runner first delta time");
        ExpectNear(secondTimes[1].elapsedTime, 1.6, "second Runner second elapsed time");
        ExpectNear(secondTimes[1].deltaTime, 0.9, "second Runner second delta time");
        Expect(a3d::testing::RunnerTestAccess::updateInfo(firstRunner).updateIndex == 1,
               "the first Runner should have its own update index");
        Expect(a3d::testing::RunnerTestAccess::updateInfo(secondRunner).updateIndex == 1,
               "the second Runner should have its own update index");
    }

    void SimulationConfigDefaults() {

        const a3d::SimulationConfig defaults;

        ExpectNear(defaults.timeStep, 1.0 / 60.0, "the default fixed delta");
        Expect(defaults.maxCatchUpSteps == 8, "the default maximum catch-up count");
        ExpectNear(defaults.timeScale, 1.0, "the default time scale");

        a3d::Scene  scene;
        a3d::Runner runner(scene);
        const auto& runnerConfiguration = runner.config();

        ExpectNear(runnerConfiguration.timeStep, defaults.timeStep, "the default Runner fixed delta");
        Expect(runnerConfiguration.maxCatchUpSteps == defaults.maxCatchUpSteps,
               "the default Runner maximum catch-up count");
        ExpectNear(runnerConfiguration.timeScale, defaults.timeScale,
                   "the default Runner configured time scale");
        ExpectNear(runner.timeScale(), defaults.timeScale, "the default Runner runtime time scale");
    }

    void SimulationConfigValidation() {

        auto expectConfigurationRejected = [](a3d::SimulationConfig configuration, std::string_view message) {
            a3d::Scene scene;
            ExpectInvalidArgument(
                [&] {
                    a3d::Runner runner(scene, configuration);
                },
                message);
        };

        auto configuration     = a3d::SimulationConfig {};
        configuration.timeStep = 0.0;
        expectConfigurationRejected(configuration, "the fixed scheduler should reject a zero fixed delta");

        configuration          = {};
        configuration.timeStep = -0.125;
        expectConfigurationRejected(configuration, "the fixed scheduler should reject a negative fixed delta");

        configuration          = {};
        configuration.timeStep = std::numeric_limits<double>::infinity();
        expectConfigurationRejected(configuration, "the fixed scheduler should reject an infinite fixed delta");

        configuration          = {};
        configuration.timeStep = std::numeric_limits<double>::quiet_NaN();
        expectConfigurationRejected(configuration, "the fixed scheduler should reject a NaN fixed delta");

        configuration                 = {};
        configuration.maxCatchUpSteps = 0;
        expectConfigurationRejected(configuration,
                                    "the fixed scheduler should reject a zero maximum catch-up count");

        configuration           = {};
        configuration.timeScale = 0.0;
        expectConfigurationRejected(configuration,
                                    "the fixed scheduler should reject a zero configured time scale");

        configuration           = {};
        configuration.timeScale = std::numeric_limits<double>::quiet_NaN();
        expectConfigurationRejected(configuration,
                                    "the fixed scheduler should reject a NaN configured time scale");

        configuration           = {};
        configuration.timeScale = std::numeric_limits<double>::infinity();
        expectConfigurationRejected(configuration,
                                    "the fixed scheduler should reject an infinite configured time scale");

        a3d::Scene  scene;
        a3d::Runner runner(scene);
        runner.timeScale(2.0);

        ExpectInvalidArgument(
            [&] {
                runner.timeScale(0.0);
            },
            "a zero runtime time scale should be rejected");
        ExpectInvalidArgument(
            [&] {
                runner.timeScale(-1.0);
            },
            "a negative runtime time scale should be rejected");
        ExpectInvalidArgument(
            [&] {
                runner.timeScale(std::numeric_limits<double>::quiet_NaN());
            },
            "a NaN runtime time scale should be rejected");
        ExpectInvalidArgument(
            [&] {
                runner.timeScale(std::numeric_limits<double>::infinity());
            },
            "an infinite runtime time scale should be rejected");
        ExpectNear(runner.timeScale(), 2.0, "rejected runtime time scales should preserve the current value");

        a3d::Scene physicsScene;
        physicsScene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        auto tinySimulationConfig =
            MakeSimulationConfig(static_cast<double>(std::numeric_limits<float>::epsilon()) * 0.5);

        ExpectInvalidArgument(
            [&] {
                a3d::Runner tinyRunner(physicsScene, tinySimulationConfig);
            },
            "a fixed delta that is effectively zero for Bullet should be rejected");
    }

    void FirstUpdateExecutesNoSimulationSteps() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        auto        node = AddMovingDynamicBody(scene);

        std::size_t runnerCallbackCount = 0;
        std::size_t simulationWillCount = 0;
        std::size_t simulationDidCount  = 0;

        a3d::Runner runner(scene, MakeSimulationConfig());
        runner.updateCallback([&runnerCallbackCount](a3d::Runner&, const a3d::Runner::UpdateInfo&) {
            ++runnerCallbackCount;
        });
        scene.willStepCallback([&simulationWillCount](a3d::Scene&, const a3d::Scene::StepInfo&) {
            ++simulationWillCount;
        });
        scene.didStepCallback([&simulationDidCount](a3d::Scene&, const a3d::Scene::StepInfo&) {
            ++simulationDidCount;
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(9000));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(9500)),
               "the first Runner update should continue");

        Expect(runnerCallbackCount == 1, "the first Runner update should invoke the Runner callback");
        Expect(simulationWillCount == 0 && simulationDidCount == 0,
               "the first Runner update should execute no simulation callbacks");
        Expect(runner.simulationStepCount() == 0,
               "the first Runner update should complete no simulation steps");
        ExpectNear(runner.simulationTime(), 0.0, "the first Runner update should not advance simulation time");
        ExpectNear(node->physicsBody()->centerOfMass().x, 0.0,
                   "the first Runner update should not advance Bullet", 1e-6);

        const auto& stats = LatestFrameStats(runner);
        Expect(stats.simulationStepCount == 0, "the first update statistics sample should report zero steps");
        ExpectNear(stats.discardedSimulationTime, 0.0,
                   "the first update statistics sample should report no discarded time");
        Expect(stats.dynamicBodies == 1,
               "a zero-step Runner update should still sample current body inventory");
        Expect(stats.primitiveShapes == 1,
               "a zero-step Runner update should still sample current shape inventory");
    }

    void SimulationRetainsFractionalAccumulator() {

        a3d::Scene                scene;
        a3d::Runner               runner(scene, MakeSimulationConfig());
        std::vector<RecordedStep> steps;
        scene.didStepCallback([&steps](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            steps.push_back(RecordStep(info));
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMicroseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(10000)),
               "the priming simulation update should continue");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(72500)),
               "the first fractional simulation update should continue");
        Expect(steps.empty(), "insufficient fixed-step time should execute no step");
        Expect(LatestFrameStats(runner).simulationStepCount == 0,
               "insufficient fixed-step time should report zero steps");

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(135000)),
               "the second fractional simulation update should continue");
        Expect(steps.size() == 1, "two retained half-step contributions should execute one step");
        ExpectStep(steps[0], 0, 0.0, 0.125, 0.125, "the accumulated simulation step");
        Expect(LatestFrameStats(runner).simulationStepCount == 1,
               "the accumulated Runner update should report one step");
        ExpectNear(runner.simulationTime(), 0.125, "the accumulated simulation time");
        Expect(runner.simulationStepCount() == 1, "the accumulated completed step count");

        runner.timeScale(2.0);
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(197500)),
               "the scaled simulation update should continue");
        Expect(steps.size() == 2, "time scale should change simulation-step accrual");
        ExpectStep(steps[1], 1, 0.125, 0.25, 0.125, "the scaled simulation step");
        ExpectNear(runner.simulationTime(), 0.25, "the scaled simulation time");
        Expect(runner.simulationStepCount() == 2, "the scaled completed step count");
        Expect(LatestFrameStats(runner).simulationStepCount == 1,
               "the scaled Runner update should report one fixed step");

        const auto& samples = a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples();
        Expect(samples.size() == 4,
               "simulation scheduling should commit one statistics record per Runner update");
    }

    void SimulationLimitsCatchUpAndReportsDiscard() {

        a3d::Scene                scene;
        a3d::Runner               runner(scene, MakeSimulationConfig(0.125, 2));
        std::vector<RecordedStep> steps;
        scene.didStepCallback([&steps](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            steps.push_back(RecordStep(info));
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMicroseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(10000)),
               "the priming catch-up update should continue");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(822500)),
               "the bounded catch-up update should continue");

        Expect(steps.size() == 2, "the catch-up limit should execute exactly two steps");
        ExpectStep(steps[0], 0, 0.0, 0.125, 0.125, "the first catch-up step");
        ExpectStep(steps[1], 1, 0.125, 0.25, 0.125, "the second catch-up step");
        ExpectNear(runner.simulationTime(), 0.25,
                   "only completed catch-up steps should advance simulation time");
        Expect(runner.simulationStepCount() == 2, "the bounded catch-up completed step count");

        const auto catchUpStats = LatestFrameStats(runner);
        Expect(catchUpStats.simulationStepCount == 2,
               "the bounded catch-up Runner update should report two steps");
        ExpectNear(catchUpStats.discardedSimulationTime, 0.5,
                   "catch-up overflow should discard only whole-step demand");

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(885000)),
               "the retained-remainder update should continue");
        Expect(steps.size() == 3, "the retained fractional remainder should complete the next step");
        ExpectStep(steps[2], 2, 0.25, 0.375, 0.125, "the retained-remainder step");
        ExpectNear(runner.simulationTime(), 0.375, "the retained-remainder simulation time");
        Expect(runner.simulationStepCount() == 3, "the retained-remainder completed step count");

        const auto& remainderStats = LatestFrameStats(runner);
        Expect(remainderStats.simulationStepCount == 1,
               "the retained-remainder Runner update should report one step");
        ExpectNear(remainderStats.discardedSimulationTime, 0.0,
                   "discarded time should reset for each update statistics batch");
    }

    void StopBetweenUpdates() {

        a3d::Scene  scene;
        a3d::Runner runner(scene);
        std::size_t callbackCount = 0;
        runner.updateCallback([&callbackCount](a3d::Runner&, const a3d::Runner::UpdateInfo&) {
            ++callbackCount;
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(6000));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(6100)),
               "the Runner should continue before stop");

        const auto infoBeforeStop = a3d::testing::RunnerTestAccess::updateInfo(runner);
        runner.stop();

        Expect(!a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(6500)),
               "a stopped Runner should not update");
        Expect(callbackCount == 1, "stop should prevent later Runner callbacks");

        const auto& infoAfterStop = a3d::testing::RunnerTestAccess::updateInfo(runner);
        Expect(infoAfterStop.updateIndex == infoBeforeStop.updateIndex,
               "stop should not advance the update index");
        ExpectNear(infoAfterStop.elapsedTime, infoBeforeStop.elapsedTime,
                   "stop should not advance elapsed time");
        ExpectNear(infoAfterStop.deltaTime, infoBeforeStop.deltaTime, "stop should not change delta time");
    }

    void StopDuringHostUpdate() {

        StopDuringHostResult result {};
        auto                 application = std::make_unique<StopDuringHostApplication>(result);

        a3d::testing::ApplicationTestAccess::prepare(*application);

        auto&      runner    = a3d::testing::ApplicationTestAccess::runner(*application);
        const auto startTime = a3d::testing::RunnerTestAccess::startTime(runner);

        Expect(a3d::testing::RunnerTestAccess::update(runner, startTime),
               "the priming host update should continue");

        const auto sampleCountBeforeStop =
            a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples().size();

        result = {};
        application->stopDuringNextHostUpdate();

        Expect(!a3d::testing::RunnerTestAccess::update(runner, startTime + std::chrono::milliseconds(125)),
               "an update stopped from hostUpdate should return false");
        Expect(result.hostUpdateCount == 1, "the stopping host callback should run exactly once");
        Expect(result.hostObservedStoppedState, "hostUpdate should observe the Runner as stopped");
        Expect(result.simulationWillCount == 0 && result.simulationDidCount == 0,
               "stop during hostUpdate should skip simulation");
        Expect(runner.simulationStepCount() == 0, "stop during hostUpdate should complete no simulation step");
        Expect(a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
               "stop during hostUpdate should complete no render frame");
        Expect(LatestFrameStats(runner).simulationStepCount == 0,
               "the stopping host update should commit zero-step statistics");
        Expect(a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples().size()
                   == sampleCountBeforeStop + 1,
               "the stopping host update should commit one statistics sample");
        Expect(runner.state() == a3d::Runner::State::Stopped,
               "the Runner should remain stopped after hostUpdate");

        Expect(!a3d::testing::RunnerTestAccess::update(runner, startTime + std::chrono::milliseconds(250)),
               "a later update should remain stopped");
        Expect(result.hostUpdateCount == 1, "a stopped Runner should not invoke hostUpdate again");

        a3d::testing::ApplicationTestAccess::shutdown(*application);
    }

    void StopDuringRunnerCallback() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

        a3d::Runner                   runner(scene, MakeSimulationConfig());
        std::vector<std::string_view> stages;
        std::size_t                   callbackCount   = 0;
        std::size_t                   simulationCount = 0;

        a3d::testing::RunnerTestAccess::start(runner, AtMicroseconds(7000000));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(7100000)),
               "the priming update should continue before the stopping callback");

        runner.updateCallback([&runner, &callbackCount, &stages](a3d::Runner&, const a3d::Runner::UpdateInfo&) {
            stages.push_back("runner");
            ++callbackCount;
            runner.stop();
        });
        scene.didStepCallback([&simulationCount, &stages](a3d::Scene&, const a3d::Scene::StepInfo&) {
            stages.push_back("simulation-did");
            ++simulationCount;
        });

        Expect(!a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(7475000)),
               "an update that stops the Runner should return false");
        Expect(callbackCount == 1, "the stopping callback should run once");
        Expect(simulationCount == 0, "stop during the Runner callback should skip pending simulation steps");
        Expect(stages.size() == 1 && stages[0] == "runner",
               "no later stage should follow the stopping Runner callback");
        Expect(runner.state() == a3d::Runner::State::Stopped, "the Runner should remain stopped");
        Expect(a3d::testing::RunnerTestAccess::updateInfo(runner).updateIndex == 1,
               "the stopping update should retain its update index");
        Expect(LatestFrameStats(runner).simulationStepCount == 0,
               "the stopping Runner update should report zero simulation steps");
        Expect(runner.simulationStepCount() == 0,
               "stop during the Runner callback should complete no simulation step");
        Expect(a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
               "stop during the Runner callback should not complete a render frame");

        Expect(!a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(7600000)),
               "a later update should remain stopped");
        Expect(callbackCount == 1, "a stopped Runner should not invoke the callback again");
        Expect(simulationCount == 0, "a stopped Runner should not invoke simulation");
    }

    void StopDuringSimulationStep() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        auto                          node = AddMovingDynamicBody(scene);

        a3d::Runner                   runner(scene, MakeSimulationConfig());
        std::vector<std::string_view> stages;
        scene.willStepCallback([&runner, &stages](a3d::Scene&, const a3d::Scene::StepInfo&) {
            stages.push_back("simulation-will");
            runner.stop();
        });
        scene.didStepCallback([&stages](a3d::Scene&, const a3d::Scene::StepInfo&) {
            stages.push_back("simulation-did");
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMicroseconds(7500000));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(7600000)),
               "the priming update should continue");
        stages.clear();

        Expect(!a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(7975000)),
               "an update stopped during a simulation step should return false");
        Expect(stages.size() == 2 && stages[0] == "simulation-will" && stages[1] == "simulation-did",
               "stop should allow the current simulation step to finish coherently");
        Expect(runner.simulationStepCount() == 1, "stop during a simulation step should prevent a second step");
        ExpectNear(runner.simulationTime(), 0.125, "the stopped Runner should complete the current step time");
        ExpectNear(node->physicsBody()->centerOfMass().x, 0.125,
                   "Bullet should complete the current step after stop", 1e-5);
        Expect(LatestFrameStats(runner).simulationStepCount == 1,
               "the stopped Runner update should report its completed step");
        Expect(runner.state() == a3d::Runner::State::Stopped,
               "the Runner should remain stopped after the simulation step");
        Expect(a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
               "stopping during simulation should not complete a render frame");
    }

    void InputUpdatesBeforeHostUpdateAndSimulation() {

        a3d::Scene                    scene;
        std::vector<std::string_view> stages;
        auto                          inputContext    = std::make_unique<RecordingInputContext>(stages);
        auto*                         inputContextPtr = inputContext.get();
        scene.inputContext(std::move(inputContext));

        RecordingApplication      application;
        std::vector<CallbackTime> hostUpdates;
        application.hostAction = [&](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
            Expect(inputContextPtr->updateCount() == hostUpdates.size() + 1,
                   "hostUpdate should observe current input state");
            stages.push_back("host");
            hostUpdates.push_back({info.elapsedTime, info.deltaTime});
        };
        application.simulationWillAction = [&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            stages.push_back("simulation-will");
        };
        application.simulationDidAction = [&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            stages.push_back("simulation-did");
        };

        a3d::Runner runner(scene, MakeSimulationConfig());
        runner.updateCallback([&](a3d::Runner& callbackRunner, const a3d::Runner::UpdateInfo& info) {
            application.invokeHostUpdate(callbackRunner, info);
        });
        scene.willStepCallback([&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            a3d::testing::ApplicationTestAccess::dispatchSceneWillStep(application, callbackScene, info);
        });
        scene.didStepCallback([&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            a3d::testing::ApplicationTestAccess::dispatchSceneDidStep(application, callbackScene, info);
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(2000));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(2100)),
               "the priming host update should continue");
        Expect(stages.size() == 2 && stages[0] == "input" && stages[1] == "host",
               "input should update before hostUpdate");

        stages.clear();
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(2225)),
               "the scheduled host update should continue");
        Expect(stages.size() == 4 && stages[0] == "input" && stages[1] == "host"
                   && stages[2] == "simulation-will" && stages[3] == "simulation-did",
               "input and hostUpdate should precede simulation");
        Expect(hostUpdates.size() == 2, "hostUpdate should run exactly once per Runner update");
        ExpectNear(hostUpdates.back().elapsedTime, 0.225, "the second hostUpdate elapsed time");
        ExpectNear(hostUpdates.back().deltaTime, 0.125, "the second hostUpdate delta time");
    }

    void NoInputContextStillRunsHostUpdate() {

        a3d::Scene scene;
        Expect(scene.inputContext() == nullptr, "the no-input fixture should not have an InputContext");

        RecordingApplication application;
        std::size_t          hostUpdateCount     = 0;
        std::size_t          simulationStepCount = 0;

        application.hostAction = [&](a3d::Runner&, const a3d::Runner::UpdateInfo&) {
            ++hostUpdateCount;
        };

        a3d::Runner runner(scene, MakeSimulationConfig());
        runner.updateCallback([&](a3d::Runner& callbackRunner, const a3d::Runner::UpdateInfo& info) {
            application.invokeHostUpdate(callbackRunner, info);
        });
        scene.didStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            ++simulationStepCount;
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(3000));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(3100)),
               "the priming no-input update should continue");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(3225)),
               "the scheduled no-input update should continue");

        Expect(hostUpdateCount == 2, "hostUpdate should run without an InputContext");
        Expect(simulationStepCount == 1, "simulation should remain scheduled without an InputContext");
    }

    void ApplicationRunnerAccessUsesPreparedRunner() {

        ApplicationRunnerAccessResult result {};

        auto                          application = std::make_unique<RunnerAccessApplication>(result);

        bool                          uninitializedAccessRejected = false;
        try {
            (void) application->runnerBeforeInitialization();
        }
        catch (const std::logic_error&) {
            uninitializedAccessRejected = true;
        }

        Expect(uninitializedAccessRejected, "Application Runner access should reject an uninitialized Runner");

        const int status = a3d::Application::Run(std::move(application));

        Expect(status == 0, "the Runner-access Application should exit successfully");
        Expect(result.hostUpdateCount == 1, "the Runner-access host callback should run once");
        Expect(result.constAndMutableAccessMatch, "const and mutable Application Runner access should match");
        Expect(result.callbackAndAccessMatch, "hostUpdate should receive the Runner executing the update");
        ExpectNear(result.initialTimeScale, 1.0, "the Runner-access initial time scale");
        ExpectNear(result.hostUpdateTimeScale, 0.5,
                   "hostUpdate should be able to change the Runner time scale");
    }

    void SceneCommandsRespectStepBoundaries() {

        a3d::Scene                    scene;
        std::vector<std::string_view> inputStages;
        auto                          inputContext    = std::make_unique<RecordingInputContext>(inputStages);
        auto*                         inputContextPtr = inputContext.get();
        scene.inputContext(std::move(inputContext));

        RecordingApplication application;
        std::vector<int>     events;
        std::size_t          commandExecutionCount = 0;

        application.hostAction = [&](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
            if (info.updateIndex == 0) {
                application.enqueueScene([&](a3d::Scene& commandScene) {
                    Expect(&commandScene == &scene, "a Scene command should receive the Scene");
                    events.push_back(1);
                    ++commandExecutionCount;

                    application.enqueueScene([&](a3d::Scene& deferredScene) {
                        Expect(&deferredScene == &scene, "a deferred Scene command should receive the Scene");
                        events.push_back(3);
                        ++commandExecutionCount;
                    });
                });
                application.enqueueScene([&](a3d::Scene& commandScene) {
                    Expect(&commandScene == &scene, "a queued Scene command should receive the Scene");
                    events.push_back(2);
                    ++commandExecutionCount;
                });
            }
        };
        application.simulationWillAction = [&](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            events.push_back(10 + static_cast<int>(info.stepIndex));

            if (info.stepIndex == 0) {
                application.enqueueScene([&](a3d::Scene& deferredScene) {
                    Expect(&deferredScene == &scene, "a sceneWillStep Scene command should receive the Scene");
                    events.push_back(4);
                    ++commandExecutionCount;
                });
            }
        };
        application.simulationDidAction = [&](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            events.push_back(20 + static_cast<int>(info.stepIndex));
        };

        a3d::Runner runner(scene, MakeSimulationConfig());
        runner.updateCallback([&](a3d::Runner& callbackRunner, const a3d::Runner::UpdateInfo& info) {
            application.invokeHostUpdate(callbackRunner, info);
        });
        scene.willStepCallback([&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            a3d::testing::ApplicationTestAccess::dispatchSceneWillStep(application, callbackScene, info);
        });
        scene.didStepCallback([&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            a3d::testing::ApplicationTestAccess::dispatchSceneDidStep(application, callbackScene, info);
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(4000));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(4100)),
               "the zero-step command-queue update should continue");
        Expect(events.empty(), "Scene commands should survive a zero-step Runner update");

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(4350)),
               "the catch-up command-queue update should continue");

        const std::vector<int> expectedCatchUpEvents {1, 2, 10, 20, 3, 4, 11, 21};
        Expect(events == expectedCatchUpEvents,
               "Scene commands should be FIFO, boundary-scoped, and deferred by batch");
        Expect(commandExecutionCount == 4, "each Scene command should execute exactly once");

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(4475)),
               "the command-free simulation update should continue");

        const std::vector<int> expectedFinalEvents {1, 2, 10, 20, 3, 4, 11, 21, 12, 22};
        Expect(events == expectedFinalEvents, "later steps should not repeat executed Scene commands");
        Expect(commandExecutionCount == 4, "Scene commands should remain exactly-once after later steps");
    }

    void RenderCommandsWaitWithoutAValidRender() {

        RecordingApplication application;
        std::size_t          commandExecutionCount = 0;
        std::size_t          renderFrameCount      = 0;
        application.enqueueRender([&](a3d::VisualWorld&) {
            ++commandExecutionCount;
        });
        application.renderAction = [&](a3d::VisualWorld&, const a3d::VisualWorld::RenderInfo&) {
            ++renderFrameCount;
        };

        a3d::Scene scene;
        Expect(scene.visualWorld() == nullptr, "the render-command fixture should not have a VisualWorld");

        a3d::Runner runner(scene);
        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(6000));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(6100)),
               "the first no-render command update should continue");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(6200)),
               "the second no-render command update should continue");
        Expect(commandExecutionCount == 0,
               "a render command should not execute without a valid render boundary");
        Expect(renderFrameCount == 0, "renderFrame should not run without a valid render boundary");
        Expect(a3d::testing::ApplicationTestAccess::renderCommandCount(application) == 1,
               "a render command should remain pending without a valid render boundary");
    }

    void RenderCommandsPrecedeRenderFrameAtAdapterBoundary() {

        AdapterRenderContext context;
        a3d::VisualWorld     visualWorld(context);
        RecordingApplication application;
        std::vector<int>     events;

        application.enqueueRender([&](a3d::VisualWorld& commandWorld) {
            Expect(&commandWorld == &visualWorld, "a render command should receive the VisualWorld");
            events.push_back(1);
            application.enqueueRender([&](a3d::VisualWorld& deferredWorld) {
                Expect(&deferredWorld == &visualWorld,
                       "a deferred render command should receive the VisualWorld");
                events.push_back(3);
            });
        });
        application.enqueueRender([&](a3d::VisualWorld&) {
            events.push_back(2);
        });
        application.renderAction = [&](a3d::VisualWorld& callbackVisualWorld,
                                       const a3d::VisualWorld::RenderInfo&) {
            Expect(&callbackVisualWorld == &visualWorld,
                   "renderFrame should receive the VisualWorld producing the frame");
            events.push_back(10);
        };

        const a3d::VisualWorld::RenderInfo info {};
        a3d::testing::ApplicationTestAccess::dispatchPendingRenderCommands(application, visualWorld, info);
        a3d::testing::ApplicationTestAccess::dispatchRenderFrame(application, visualWorld, info);

        const std::vector<int> firstBoundary {1, 2, 10};
        Expect(events == firstBoundary, "pending render commands should execute before renderFrame");
        Expect(a3d::testing::ApplicationTestAccess::renderCommandCount(application) == 1,
               "a render command queued while draining should wait for the next frame");

        a3d::testing::ApplicationTestAccess::dispatchPendingRenderCommands(application, visualWorld, info);
        a3d::testing::ApplicationTestAccess::dispatchRenderFrame(application, visualWorld, info);

        const std::vector<int> secondBoundary {1, 2, 10, 3, 10};
        Expect(events == secondBoundary,
               "the deferred render command should execute once at the next boundary");
    }

    void SimulationCallbacksRunInOrderWithPhysics() {

        a3d::Scene                    scene;
        std::vector<std::string_view> stages;
        auto                          inputContext    = std::make_unique<RecordingInputContext>(stages);
        auto*                         inputContextPtr = inputContext.get();
        scene.inputContext(std::move(inputContext));
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        auto                 bodyNode = AddMovingDynamicBody(scene);
        RecordingApplication application;

        a3d::Runner          runner(scene, MakeSimulationConfig());
        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(8000));

        // Prime the Runner so the observed Runner update has a nonzero delta.
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(8100)),
               "the priming Runner update should continue");
        stages.clear();
        inputContextPtr->reset();

        std::vector<CallbackTime> hostUpdateTimes;
        std::vector<RecordedStep> simulationSteps;
        double                    positionBeforePhysics = -1.0;
        double                    positionAfterPhysics  = -1.0;
        application.hostAction = [&](a3d::Runner& callbackRunner, const a3d::Runner::UpdateInfo& info) {
            Expect(&callbackRunner == &runner, "hostUpdate should receive the Runner executing the update");
            stages.push_back("host");
            hostUpdateTimes.push_back({info.elapsedTime, info.deltaTime});
        };
        application.simulationWillAction = [&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            Expect(&callbackScene == &scene, "sceneWillStep should receive the Scene being stepped");
            stages.push_back("simulation-will");
            simulationSteps.push_back(RecordStep(info));
            positionBeforePhysics = bodyNode->physicsBody()->centerOfMass().x;
        };
        application.simulationDidAction = [&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            Expect(&callbackScene == &scene,
                   "sceneDidStep should receive the same Scene that completed the step");
            stages.push_back("simulation-did");
            simulationSteps.push_back(RecordStep(info));
            positionAfterPhysics = bodyNode->physicsBody()->centerOfMass().x;
            ExpectNear(runner.simulationTime(), 0.0,
                       "Runner simulation time should not advance inside callbacks");
            Expect(runner.simulationStepCount() == 0, "Runner step count should not advance inside callbacks");
        };
        runner.updateCallback([&](a3d::Runner& callbackRunner, const a3d::Runner::UpdateInfo& info) {
            application.invokeHostUpdate(callbackRunner, info);
        });
        scene.willStepCallback([&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            a3d::testing::ApplicationTestAccess::dispatchSceneWillStep(application, callbackScene, info);
        });
        scene.didStepCallback([&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            a3d::testing::ApplicationTestAccess::dispatchSceneDidStep(application, callbackScene, info);
        });

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(8225)),
               "the observed Runner update should continue");

        Expect(stages.size() == 4 && stages[0] == "input" && stages[1] == "host"
                   && stages[2] == "simulation-will" && stages[3] == "simulation-did",
               "host, simulation, and internal physics should run in order");
        Expect(inputContextPtr->updateCount() == 1,
               "input should update exactly once in the observed Runner update");
        Expect(hostUpdateTimes.size() == 1, "hostUpdate should run exactly once");
        Expect(simulationSteps.size() == 2, "each simulation callback should run exactly once");
        ExpectNear(hostUpdateTimes[0].elapsedTime, 0.225, "the hostUpdate elapsed time");
        ExpectNear(hostUpdateTimes[0].deltaTime, 0.125, "the hostUpdate delta time");
        ExpectNear(positionBeforePhysics, 0.0, "sceneWillStep should observe state before Bullet");
        ExpectNear(positionAfterPhysics, 0.125, "sceneDidStep should observe state after Bullet", 1e-5);
        for (const auto& step : simulationSteps) {
            ExpectStep(step, 0, 0.0, 0.125, 0.125, "the callback simulation step");
        }
        ExpectNear(runner.simulationTime(), 0.125, "Runner simulation time after the callback pipeline");
        Expect(runner.simulationStepCount() == 1, "Runner step count after the callback pipeline");
        Expect(LatestFrameStats(runner).simulationStepCount == 1,
               "the callback Runner update should report one step");
    }

    void NeitherWorldPipeline() {

        a3d::Scene                    scene;
        std::vector<std::string_view> stages;
        auto                          inputContext    = std::make_unique<RecordingInputContext>(stages);
        auto*                         inputContextPtr = inputContext.get();
        scene.inputContext(std::move(inputContext));
        RecordingApplication application;

        Expect(scene.physicsWorld() == nullptr, "the neither-world fixture should not have a PhysicsWorld");
        Expect(scene.visualWorld() == nullptr, "the neither-world fixture should not have a VisualWorld");

        a3d::Runner               runner(scene);
        std::vector<CallbackTime> hostUpdateTimes;

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(10000));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(10100)),
               "the priming neither-world Runner update should continue");
        stages.clear();
        inputContextPtr->reset();

        application.hostAction = [&](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
            stages.push_back("host");
            hostUpdateTimes.push_back({info.elapsedTime, info.deltaTime});
        };
        application.simulationWillAction = [&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            stages.push_back("simulation-will");
        };
        application.simulationDidAction = [&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            stages.push_back("simulation-did");
        };
        runner.updateCallback([&](a3d::Runner& callbackRunner, const a3d::Runner::UpdateInfo& info) {
            application.invokeHostUpdate(callbackRunner, info);
        });
        scene.willStepCallback([&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            a3d::testing::ApplicationTestAccess::dispatchSceneWillStep(application, callbackScene, info);
        });
        scene.didStepCallback([&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            a3d::testing::ApplicationTestAccess::dispatchSceneDidStep(application, callbackScene, info);
        });

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(10125)),
               "the observed neither-world Runner update should continue");

        Expect(stages.size() == 4 && stages[0] == "input" && stages[1] == "host"
                   && stages[2] == "simulation-will" && stages[3] == "simulation-did",
               "Scene simulation should run without either world");
        Expect(inputContextPtr->updateCount() == 1, "input should update once without either world");
        Expect(hostUpdateTimes.size() == 1, "hostUpdate should run once without either world");
        ExpectNear(hostUpdateTimes[0].elapsedTime, 0.125, "the neither-world callback elapsed time");
        ExpectNear(hostUpdateTimes[0].deltaTime, 0.025, "the neither-world callback delta time");
        Expect(runner.simulationStepCount() == 1,
               "the neither-world Scene should complete one simulation step");
        ExpectNear(runner.simulationTime(), runner.config().timeStep, "the neither-world simulation time");
        Expect(LatestFrameStats(runner).simulationStepCount == 1,
               "the neither-world Runner update should report one step");
    }

    void NoVisualWorldHasZeroCompletedRenderFrames() {

        a3d::Scene  scene;
        a3d::Runner runner(scene);

        Expect(scene.visualWorld() == nullptr, "the no-visual fixture should not have a VisualWorld");
        Expect(a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
               "a new Runner should have no completed render frames");

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(10500));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(10600)),
               "the first no-visual Runner update should continue");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(10800)),
               "the second no-visual Runner update should continue");

        Expect(a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
               "Runner updates without a VisualWorld should not complete render frames");
    }

    void SceneRootInvariant() {

        a3d::Scene scene;
        auto       originalRoot = scene.rootNode();

        Expect(originalRoot != nullptr, "Scene should start with a non-null root Node");
        Expect(originalRoot->scene() == &scene, "the initial root Node should be attached to its Scene");

        scene.rootNode(originalRoot);
        Expect(scene.rootNode() == originalRoot, "installing the current root should be a no-op");

        bool rejected = false;
        try {
            scene.rootNode(nullptr);
        }
        catch (const std::invalid_argument&) {
            rejected = true;
        }

        Expect(rejected, "Scene should reject a null root Node");
        Expect(scene.rootNode() == originalRoot,
               "a rejected root replacement should preserve the original root");
        Expect(originalRoot->scene() == &scene,
               "a rejected root replacement should leave the original root attached");
    }

    void PhysicsInventorySamplingIsCurrent() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

        auto staticNode = std::make_shared<a3d::Node>("static body");
        staticNode
            ->physicsBody(std::make_unique<a3d::PhysicsBody>(a3d::PhysicsBody::Type::Static,
                                                             std::make_shared<a3d::SpherePhysicsShape>(1.0f)));
        scene.rootNode()->addChild(staticNode);

        auto dynamicNode = std::make_shared<a3d::Node>("dynamic body");
        dynamicNode
            ->physicsBody(std::make_unique<a3d::PhysicsBody>(a3d::PhysicsBody::Type::Dynamic,
                                                             std::make_shared<a3d::SpherePhysicsShape>(1.0f)));
        scene.rootNode()->addChild(dynamicNode);

        auto kinematicNode = std::make_shared<a3d::Node>("kinematic body");
        kinematicNode
            ->physicsBody(std::make_unique<a3d::PhysicsBody>(a3d::PhysicsBody::Type::Kinematic,
                                                             std::make_shared<a3d::SpherePhysicsShape>(1.0f)));
        scene.rootNode()->addChild(kinematicNode);

        const auto first = scene.physicsWorld()->inventory();

        Expect(first.staticBodies == 1, "current static-body inventory");
        Expect(first.dynamicBodies == 1, "current dynamic-body inventory");
        Expect(first.kinematicBodies == 1, "current kinematic-body inventory");
        Expect(first.primitiveShapes == 3, "current primitive-shape inventory");
        Expect(first.boundingBoxShapes == 0, "current bounding-box-shape inventory");
        Expect(first.convexHullShapes == 0, "current convex-hull-shape inventory");
        Expect(first.concavePolyhedronShapes == 0, "current concave-polyhedron-shape inventory");

        const auto second = scene.physicsWorld()->inventory();

        Expect(second.staticBodies == first.staticBodies && second.dynamicBodies == first.dynamicBodies
                   && second.kinematicBodies == first.kinematicBodies,
               "repeated queries should not multiply body inventory");
        Expect(second.primitiveShapes == first.primitiveShapes
                   && second.boundingBoxShapes == first.boundingBoxShapes
                   && second.convexHullShapes == first.convexHullShapes
                   && second.concavePolyhedronShapes == first.concavePolyhedronShapes,
               "repeated queries should not multiply shape inventory");
    }

    void PhysicsInventoryAcrossMultipleSteps() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        AddMovingDynamicBody(scene, 0.0);

        std::size_t completedStepCount = 0;
        scene.didStepCallback([&completedStepCount](a3d::Scene&, const a3d::Scene::StepInfo&) {
            ++completedStepCount;
        });

        a3d::Runner runner(scene, MakeSimulationConfig());
        a3d::testing::RunnerTestAccess::start(runner, AtMicroseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(10000)),
               "the priming inventory update should continue");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(385000)),
               "the multi-step inventory update should continue");

        Expect(completedStepCount == 3, "the multi-step Runner update should complete three Scene steps");
        const auto& stats = LatestFrameStats(runner);
        Expect(stats.simulationStepCount == 3, "the multi-step Runner update should report three steps");
        Expect(stats.dynamicBodies == 1, "multiple physics steps should not multiply body inventory");
        Expect(stats.primitiveShapes == 1, "multiple physics steps should not multiply shape inventory");

        const auto& samples = a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples();
        Expect(samples.size() == 2, "multiple physics steps should commit one update statistics sample");
    }

    void PhysicsInventoryUsesLatestStep() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        auto dynamicNode = AddMovingDynamicBody(scene, 0.0);

        scene.didStepCallback([&dynamicNode](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            if (info.stepIndex == 0) {
                dynamicNode->physicsBody(std::unique_ptr<a3d::PhysicsBody> {});
            }
        });

        a3d::Runner runner(scene, MakeSimulationConfig());
        a3d::testing::RunnerTestAccess::start(runner, AtMicroseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(10000)),
               "the priming latest-inventory update should continue");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(260000)),
               "the two-step latest-inventory update should continue");

        const auto& stats = LatestFrameStats(runner);
        Expect(stats.simulationStepCount == 2, "the latest-inventory update should report two steps");
        Expect(stats.dynamicBodies == 0, "update statistics should retain the latest physics inventory");
        Expect(stats.primitiveShapes == 0, "update statistics should retain the latest shape inventory");
    }

    void PhysicsInventoryPrecedesDidStep() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

        auto dynamicNode = std::make_shared<a3d::Node>("dynamic body");
        dynamicNode
            ->physicsBody(std::make_unique<a3d::PhysicsBody>(a3d::PhysicsBody::Type::Dynamic,
                                                             std::make_shared<a3d::SpherePhysicsShape>(1.0f)));
        scene.rootNode()->addChild(dynamicNode);

        std::size_t callbackCount = 0;
        scene.didStepCallback([&callbackCount, &dynamicNode](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            ++callbackCount;
            ExpectStep(RecordStep(info), 0, 0.0, 0.125, 0.125, "the inventory-order physics step");
            dynamicNode->physicsBody(std::unique_ptr<a3d::PhysicsBody> {});
        });

        a3d::Runner runner(scene, MakeSimulationConfig());
        a3d::testing::RunnerTestAccess::start(runner, AtMicroseconds(11000000));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(11100000)),
               "the priming inventory-order update should continue");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(11225000)),
               "the inventory-order Runner update should continue");

        Expect(callbackCount == 1, "sceneDidStep should run exactly once");

        const auto& samples = a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples();
        Expect(samples.size() == 2, "each inventory-order Runner update should commit one statistics sample");

        const auto& stats = std::get<1>(samples.back());
        Expect(stats.dynamicBodies == 1, "the completed-step inventory should precede callback mutation");
        Expect(stats.primitiveShapes == 1,
               "the completed-step shape inventory should precede callback mutation");

        const auto current = scene.physicsWorld()->inventory();
        Expect(current.dynamicBodies == 0, "the callback mutation should affect the current body inventory");
        Expect(current.primitiveShapes == 0, "the callback mutation should affect the current shape inventory");
    }

    void BulletAdvancesExactlyOncePerSimulationStep() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        auto        node = AddMovingDynamicBody(scene);

        std::size_t completedStepCount = 0;
        scene.didStepCallback([&completedStepCount](a3d::Scene&, const a3d::Scene::StepInfo&) {
            ++completedStepCount;
        });

        a3d::Runner runner(scene, MakeSimulationConfig());
        a3d::testing::RunnerTestAccess::start(runner, AtMicroseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(10000)),
               "the priming exact-step update should continue");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(135000)),
               "the exact-step update should continue");

        Expect(completedStepCount == 1, "one simulation step should complete once");
        ExpectNear(node->physicsBody()->centerOfMass().x, 0.125,
                   "one simulation step should advance Bullet by the full requested delta", 1e-5);

        node->physicsBody()->applyForce({4.0f, 0.0f, 0.0f}, false);
        ExpectNear(node->physicsBody()->totalForce().x, 4.0,
                   "the force fixture should hold a force before stepping", 1e-6);

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(260000)),
               "the force-clearing exact-step update should continue");
        Expect(completedStepCount == 2, "the second simulation step should complete once");
        ExpectNear(node->physicsBody()->totalForce().x, 0.0,
                   "an exact Bullet step should preserve force clearing", 1e-6);
    }

    void BulletRejectsInvalidStepDelta() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        a3d::Profiler    profiler;
        constexpr double tinyDeltaTime = 1e-9;

        ExpectInvalidArgument(
            [&] {
                scene.physicsWorld()->step(tinyDeltaTime, profiler);
            },
            "PhysicsWorld should reject a delta that is effectively zero for Bullet");
    }

    struct SimulationScheduleResult {
        std::vector<RecordedStep> steps;
        double                    bodyPosition;
        double                    simulationTime;
        std::uint64_t             simulationStepCount;
    };

    SimulationScheduleResult RunSimulationPhysicsSchedule(const std::vector<std::int64_t>&
                                                              updateDeltasMicroseconds) {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        auto                     node = AddMovingDynamicBody(scene);
        a3d::Runner              runner(scene, MakeSimulationConfig());

        SimulationScheduleResult result;
        scene.didStepCallback([&result](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            result.steps.push_back(RecordStep(info));
        });

        std::int64_t now = 10000;
        a3d::testing::RunnerTestAccess::start(runner, AtMicroseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(now)),
               "the repeatability priming update should continue");

        for (const auto delta : updateDeltasMicroseconds) {
            now += delta;
            Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(now)),
                   "the repeatability scheduled update should continue");
        }

        result.bodyPosition        = node->physicsBody()->centerOfMass().x;
        result.simulationTime      = runner.simulationTime();
        result.simulationStepCount = runner.simulationStepCount();
        return result;
    }

    void SimulationPhysicsIsRepeatable() {

        const auto catchUp     = RunSimulationPhysicsSchedule({375000});
        const auto distributed = RunSimulationPhysicsSchedule({125000, 125000, 125000});

        Expect(catchUp.steps.size() == 3, "the catch-up schedule should execute three steps");
        Expect(distributed.steps.size() == 3, "the distributed schedule should execute three steps");

        for (std::size_t index = 0; index < catchUp.steps.size(); ++index) {
            const double startTime = static_cast<double>(index) * 0.125;
            const double endTime   = static_cast<double>(index + 1) * 0.125;

            ExpectStep(catchUp.steps[index], index, startTime, endTime, 0.125,
                       "the catch-up repeatability step");
            ExpectStep(distributed.steps[index], index, startTime, endTime, 0.125,
                       "the distributed repeatability step");
        }

        ExpectNear(catchUp.bodyPosition, distributed.bodyPosition,
                   "fixed-step physics state should not depend on update grouping", 1e-5);
        ExpectNear(catchUp.bodyPosition, 0.375, "three exact fixed steps should advance the body three deltas",
                   1e-5);
        ExpectNear(catchUp.simulationTime, distributed.simulationTime,
                   "repeatable schedules should reach the same simulation time");
        Expect(catchUp.simulationStepCount == distributed.simulationStepCount
                   && catchUp.simulationStepCount == 3,
               "repeatable schedules should complete the same step count");
    }

    void SimulationAtOneHundredTwentyHertzDoesNotDrift() {

        constexpr double FIXED_DELTA_TIME = 1.0 / 120.0;

        a3d::Scene       scene;

        a3d::Runner      runner(scene, MakeSimulationConfig(FIXED_DELTA_TIME));

        a3d::testing::RunnerTestAccess::start(runner, AtMicroseconds(0));

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(0)),
               "the priming 120 Hz update should continue");

        for (std::int64_t millisecond = 1; millisecond <= 1000; ++millisecond) {

            Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(millisecond * 1000)),
                   "the 120 Hz accumulation update should continue");
        }

        Expect(runner.simulationStepCount() == 120, "one second should execute 120 fixed simulation steps");

        ExpectNear(runner.simulationTime(), 1.0, "120 fixed steps should advance one simulation second", 1e-12);
    }

    void PauseSimulationPreconditions() {

        a3d::Scene  scene;
        a3d::Runner runner(scene);

        Expect(!runner.simulationPaused(), "a new Runner should not be paused");
        ExpectLogicError(
            [&] {
                runner.pauseSimulation();
            },
            "pause should reject an idle Runner");

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(0));
        runner.pauseSimulation();
        runner.pauseSimulation();
        Expect(runner.simulationPaused(), "pause should be supported and idempotent");

        runner.stop();
        ExpectLogicError(
            [&] {
                runner.pauseSimulation();
            },
            "pause should reject a stopped Runner");
    }

    void PausedUpdatesKeepInputHostAndStatisticsActive() {

        a3d::Scene                    scene;
        std::vector<std::string_view> stages;
        auto                          inputContext    = std::make_unique<RecordingInputContext>(stages);
        auto*                         inputContextPtr = inputContext.get();
        scene.inputContext(std::move(inputContext));
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        AddMovingDynamicBody(scene, 0.0);

        RecordingApplication application;
        a3d::Runner          runner(scene, MakeSimulationConfig());
        std::size_t          hostUpdateCount = 0;
        std::size_t          simulationCount = 0;

        application.hostAction = [&](a3d::Runner&, const a3d::Runner::UpdateInfo&) {
            stages.push_back("host");
            ++hostUpdateCount;
        };
        runner.updateCallback([&](a3d::Runner& callbackRunner, const a3d::Runner::UpdateInfo& info) {
            application.invokeHostUpdate(callbackRunner, info);
        });
        scene.didStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            ++simulationCount;
        });
        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(0)),
               "the priming paused-pipeline update should continue");

        stages.clear();
        inputContextPtr->reset();
        hostUpdateCount        = 0;
        const auto sampleCount = a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples().size();
        runner.pauseSimulation();

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(1000)),
               "a paused Runner update should continue");
        Expect(stages.size() == 2 && stages[0] == "input" && stages[1] == "host",
               "paused updates should retain input-before-host order");
        Expect(inputContextPtr->updateCount() == 1 && hostUpdateCount == 1,
               "paused updates should execute input and hostUpdate once");
        Expect(simulationCount == 0, "paused updates should execute no automatic simulation");
        ExpectNear(runner.simulationTime(), 0.0, "paused simulation time");
        Expect(runner.simulationStepCount() == 0, "paused simulation step count");
        Expect(a3d::testing::RunnerTestAccess::updateInfo(runner).updateIndex == 1,
               "paused updates should advance the Runner update index");
        ExpectNear(a3d::testing::RunnerTestAccess::updateInfo(runner).elapsedTime, 1.0,
                   "paused updates should advance elapsed update time");
        ExpectNear(a3d::testing::RunnerTestAccess::updateInfo(runner).deltaTime, 1.0,
                   "paused updates should retain normal update deltas");
        Expect(a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples().size() == sampleCount + 1,
               "paused updates should commit one statistics sample");

        const auto& stats = LatestFrameStats(runner);
        Expect(stats.simulationStepCount == 0, "paused statistics should report zero steps");
        ExpectNear(stats.discardedSimulationTime, 0.0, "paused statistics should report no discarded time");
        Expect(stats.dynamicBodies == 1, "paused updates should sample current physics inventory");
        Expect(a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
               "the headless paused fixture should complete no render frame");
    }

    void PauseClearsFixedAccumulatorWithoutDiscard() {

        a3d::Scene  scene;
        a3d::Runner runner(scene, MakeSimulationConfig());
        std::size_t stepCount = 0;
        scene.didStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            ++stepCount;
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMicroseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(0)),
               "the priming accumulator-pause update should continue");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(62500)),
               "the fractional accumulator update should continue");
        ExpectNear(a3d::testing::RunnerTestAccess::simulationAccumulator(runner), 0.0625,
                   "the seeded fixed accumulator");

        runner.pauseSimulation();
        ExpectNear(a3d::testing::RunnerTestAccess::simulationAccumulator(runner), 0.0,
                   "pause should clear the entire fixed accumulator");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMicroseconds(1062500)),
               "the long paused accumulator update should continue");
        Expect(stepCount == 0, "paused wall time should create no fixed step");
        ExpectNear(a3d::testing::RunnerTestAccess::simulationAccumulator(runner), 0.0,
                   "paused wall time should not rebuild the accumulator");
        ExpectNear(LatestFrameStats(runner).discardedSimulationTime, 0.0,
                   "clearing the accumulator for pause is not discarded simulation time");
    }

    void PauseDuringHostUpdateSuppressesScheduling() {

        a3d::Scene                    scene;
        std::vector<std::string_view> stages;
        auto                          inputContext    = std::make_unique<RecordingInputContext>(stages);
        auto*                         inputContextPtr = inputContext.get();
        scene.inputContext(std::move(inputContext));
        RecordingApplication application;
        a3d::Runner          runner(scene, MakeSimulationConfig());
        bool                 pauseNow        = false;
        bool                 resumeNow       = false;
        std::size_t          hostUpdateCount = 0;
        std::size_t          stepCount       = 0;

        application.hostAction = [&](a3d::Runner&, const a3d::Runner::UpdateInfo&) {
            Expect(inputContextPtr->updateCount() == hostUpdateCount + 1,
                   "hostUpdate controls should observe current input");
            ++hostUpdateCount;
            if (pauseNow) {
                runner.pauseSimulation();
            }
            else if (resumeNow) {
                runner.resumeSimulation();
            }
        };
        runner.updateCallback([&](a3d::Runner& callbackRunner, const a3d::Runner::UpdateInfo& info) {
            application.invokeHostUpdate(callbackRunner, info);
        });
        scene.didStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            ++stepCount;
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(0)),
               "the priming host-control update should continue");
        pauseNow = true;

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(125)),
               "pause during hostUpdate should keep the Runner alive");
        Expect(runner.simulationPaused() && hostUpdateCount == 2 && stepCount == 0,
               "hostUpdate pause should suppress same-update scheduling");

        pauseNow  = false;
        resumeNow = true;
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(250)),
               "resume during hostUpdate should keep the Runner alive");
        Expect(!runner.simulationPaused() && hostUpdateCount == 3 && stepCount == 0,
               "hostUpdate resume should suppress same-update scheduling");

        resumeNow = false;
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(375)),
               "automatic scheduling should resume after hostUpdate suppression");
        Expect(hostUpdateCount == 4 && stepCount == 1,
               "hostUpdate resume should suppress exactly one automatic update");
    }

    void PauseDuringCatchUpFinishesOnlyCurrentStep() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        auto                          bodyNode = AddMovingDynamicBody(scene);
        a3d::Runner                   runner(scene, MakeSimulationConfig());
        std::vector<std::string_view> stages;

        scene.willStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            stages.push_back("simulation-will");
            runner.pauseSimulation();
        });
        scene.didStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            stages.push_back("simulation-did");
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(0)),
               "the priming catch-up pause update should continue");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(375)),
               "pause during catch-up should keep the Runner alive");

        Expect(stages.size() == 2 && stages[0] == "simulation-will" && stages[1] == "simulation-did",
               "pause during catch-up should finish the current step coherently");
        Expect(runner.simulationPaused() && runner.simulationStepCount() == 1,
               "pause during catch-up should abandon remaining steps");
        ExpectNear(bodyNode->physicsBody()->centerOfMass().x, 0.125,
                   "pause during catch-up should advance Bullet once", 1e-5);
        ExpectNear(a3d::testing::RunnerTestAccess::simulationAccumulator(runner), 0.0,
                   "pause during catch-up should clear all accumulated demand");
        ExpectNear(LatestFrameStats(runner).discardedSimulationTime, 0.0,
                   "pause-cleared catch-up demand should not count as discarded time");
    }

    void ResumeClearsRequestsAndSuppressesOneAutomaticUpdate() {

        {
            a3d::Scene                scene;
            a3d::Runner               runner(scene, MakeSimulationConfig());
            std::vector<RecordedStep> steps;
            scene.didStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo& info) {
                steps.push_back(RecordStep(info));
            });

            ExpectLogicError(
                [&] {
                    runner.resumeSimulation();
                },
                "resume should reject an idle Runner");
            a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(0));
            runner.resumeSimulation();
            Expect(!runner.simulationPaused(), "resume should be a no-op when simulation is already running");
            Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(0)),
                   "the priming resume update should continue");

            runner.pauseSimulation();
            runner.requestSimulationStep();
            runner.requestSimulationStep();
            runner.resumeSimulation();
            Expect(!runner.simulationPaused(), "resume should clear paused state");
            Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(1000)),
                   "the suppressed resume update should continue");
            Expect(steps.empty(), "resume should clear requests and suppress one automatic update");
            ExpectNear(a3d::testing::RunnerTestAccess::simulationAccumulator(runner), 0.0,
                       "the suppressed resume update should add no accumulator demand");

            runner.pauseSimulation();
            Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(1125)),
                   "the re-paused cleared-request update should continue");
            Expect(steps.empty(), "cleared requested steps should not execute after re-pausing");

            runner.resumeSimulation();
            Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(2000)),
                   "the second suppressed resume update should continue");
            Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(2125)),
                   "the first post-resume automatic update should continue");
            Expect(steps.size() == 1, "automatic simulation scheduling should resume after one suppression");

            runner.stop();
            ExpectLogicError(
                [&] {
                    runner.resumeSimulation();
                },
                "resume should reject a stopped Runner");
        }
    }

    void RequestedStepPreconditions() {

        {
            a3d::Scene  scene;
            a3d::Runner runner(scene, MakeSimulationConfig());
            ExpectLogicError(
                [&] {
                    runner.requestSimulationStep();
                },
                "a requested step should reject an idle Runner");
        }

        {
            a3d::Scene  scene;
            a3d::Runner runner(scene, MakeSimulationConfig());
            a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(0));
            ExpectLogicError(
                [&] {
                    runner.requestSimulationStep();
                },
                "a requested step should require paused simulation");

            runner.pauseSimulation();
            runner.requestSimulationStep();
            runner.stop();
            ExpectLogicError(
                [&] {
                    runner.requestSimulationStep();
                },
                "a requested step should reject a stopped Runner");
        }
    }

    void RequestedStepsUseFixedDeltaAndIgnoreTimeScale() {

        a3d::Scene                    scene;
        std::vector<std::string_view> updateStages;
        auto                          inputContext    = std::make_unique<RecordingInputContext>(updateStages);
        auto*                         inputContextPtr = inputContext.get();
        scene.inputContext(std::move(inputContext));
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        auto                      bodyNode = AddMovingDynamicBody(scene);
        RecordingApplication      application;
        a3d::Runner               runner(scene, MakeSimulationConfig(0.125, 1));
        std::size_t               hostUpdateCount = 0;
        std::vector<RecordedStep> simulationSteps;

        application.hostAction = [&](a3d::Runner&, const a3d::Runner::UpdateInfo&) {
            updateStages.push_back("host");
            ++hostUpdateCount;
        };
        runner.updateCallback([&](a3d::Runner& callbackRunner, const a3d::Runner::UpdateInfo& info) {
            application.invokeHostUpdate(callbackRunner, info);
        });
        scene.didStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            simulationSteps.push_back(RecordStep(info));
        });
        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(0)),
               "the priming requested-step update should continue");
        updateStages.clear();
        inputContextPtr->reset();
        hostUpdateCount = 0;
        const auto sampleCountBeforeRequests =
            a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples().size();
        runner.timeScale(3.0);
        runner.pauseSimulation();
        runner.requestSimulationStep();
        runner.requestSimulationStep();
        runner.requestSimulationStep();

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(1000)),
               "the requested-step update should continue");
        Expect(runner.simulationPaused(), "requested steps should leave simulation paused");
        Expect(simulationSteps.size() == 3, "three requests should execute three simulation steps");
        Expect(inputContextPtr->updateCount() == 1 && hostUpdateCount == 1 && updateStages.size() == 2
                   && updateStages[0] == "input" && updateStages[1] == "host",
               "input and hostUpdate should remain once-per-Runner-update work");

        for (std::size_t index = 0; index < simulationSteps.size(); ++index) {

            const double startTime = static_cast<double>(index) * 0.125;
            const double endTime   = static_cast<double>(index + 1) * 0.125;
            ExpectStep(simulationSteps[index], index, startTime, endTime, 0.125,
                       "the requested simulation step");
        }

        ExpectNear(runner.simulationTime(), 0.375, "requested-step simulation time");
        Expect(runner.simulationStepCount() == 3, "requested-step completed count");
        ExpectNear(bodyNode->physicsBody()->centerOfMass().x, 0.375,
                   "requested steps should advance Bullet exactly three times", 1e-5);
        ExpectNear(a3d::testing::RunnerTestAccess::simulationAccumulator(runner), 0.0,
                   "requested steps should not use the automatic accumulator");
        Expect(LatestFrameStats(runner).simulationStepCount == 3,
               "requested-step statistics should report actual completed steps");
        Expect(a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples().size()
                   == sampleCountBeforeRequests + 1,
               "requested steps should share one Runner-update profiling batch");
        Expect(LatestFrameStats(runner).dynamicBodies == 1,
               "requested-step statistics should retain current inventory");
        Expect(a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
               "the headless requested-step update should complete no render frame");
    }

    void RequestedStepsPreserveCommandAndCallbackOrder() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        auto                 bodyNode = AddMovingDynamicBody(scene);
        RecordingApplication application;
        std::vector<int>     events;

        application.enqueueScene([&](a3d::Scene&) {
            events.push_back(1);
            application.enqueueScene([&](a3d::Scene&) {
                events.push_back(7);
            });
        });
        application.enqueueRender([&](a3d::VisualWorld&) {
            events.push_back(99);
        });
        application.simulationWillAction = [&](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            events.push_back(10 + static_cast<int>(info.stepIndex));
        };
        application.simulationDidAction = [&](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            events.push_back(40 + static_cast<int>(info.stepIndex));
        };

        scene.willStepCallback([&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            a3d::testing::ApplicationTestAccess::dispatchSceneWillStep(application, callbackScene, info);
        });
        scene.didStepCallback([&](a3d::Scene& callbackScene, const a3d::Scene::StepInfo& info) {
            a3d::testing::ApplicationTestAccess::dispatchSceneDidStep(application, callbackScene, info);
        });

        a3d::Runner runner(scene, MakeSimulationConfig());
        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(0));
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(0)),
               "the priming requested-order update should continue");
        runner.pauseSimulation();
        runner.requestSimulationStep();
        runner.requestSimulationStep();
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(125)),
               "the requested-order update should continue");

        const std::vector<int> expected {1, 10, 40, 7, 11, 41};
        Expect(events == expected, "requested steps should preserve command and callback boundaries");
        Expect(a3d::testing::ApplicationTestAccess::sceneCommandCount(application) == 0,
               "requested steps should consume the Scene command queue");
        ExpectNear(bodyNode->physicsBody()->centerOfMass().x, 0.25,
                   "requested steps should execute internal physics between Scene callbacks", 1e-5);
        Expect(a3d::testing::ApplicationTestAccess::renderCommandCount(application) == 1,
               "a render command should remain queued without a valid render");
    }

    void RequestedStepSnapshotDefersCallbackRequests() {

        a3d::Scene                scene;
        a3d::Runner               runner(scene, MakeSimulationConfig());
        std::vector<RecordedStep> steps;

        scene.willStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            if (info.stepIndex == 0) {
                runner.requestSimulationStep();
            }
        });
        scene.didStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            steps.push_back(RecordStep(info));
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(0));
        runner.pauseSimulation();
        runner.requestSimulationStep();

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(0)),
               "the first snapshot-request update should continue");
        Expect(steps.size() == 1, "a request created during a step should not extend its snapshot");
        Expect(LatestFrameStats(runner).simulationStepCount == 1,
               "the first snapshot statistics should report one step");

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(125)),
               "the deferred requested-step update should continue");
        Expect(steps.size() == 2, "the callback-created request should execute next update");
        ExpectStep(steps.back(), 1, 0.125, 0.25, 0.125, "the deferred requested step");
    }

    void StopInterruptsRequestedStepsAfterCurrentStep() {

        a3d::Scene scene;
        scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
        auto                          bodyNode = AddMovingDynamicBody(scene);
        a3d::Runner                   runner(scene, MakeSimulationConfig());
        std::vector<std::string_view> stages;

        scene.willStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            stages.push_back("simulation-will");
            runner.stop();
        });
        scene.didStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo&) {
            stages.push_back("simulation-did");
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(0));
        runner.pauseSimulation();
        runner.requestSimulationStep();
        runner.requestSimulationStep();
        runner.requestSimulationStep();

        Expect(!a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(0)),
               "a stop-interrupted requested update should return false");
        Expect(stages.size() == 2 && stages[0] == "simulation-will" && stages[1] == "simulation-did",
               "stop should allow the current requested step to finish coherently");
        Expect(runner.simulationStepCount() == 1, "stop should abandon remaining requested steps");
        ExpectNear(bodyNode->physicsBody()->centerOfMass().x, 0.125,
                   "stop-interrupted requests should advance Bullet once", 1e-5);
        Expect(LatestFrameStats(runner).simulationStepCount == 1,
               "stop-interrupted request statistics should report one step");
        Expect(a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
               "stop-interrupted requested steps should skip rendering");
    }

    void ResumeInterruptsRequestedStepsAndClearsRemainder() {

        a3d::Scene                scene;
        a3d::Runner               runner(scene, MakeSimulationConfig());
        std::vector<RecordedStep> steps;

        scene.willStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            if (info.stepIndex == 0) {
                runner.resumeSimulation();
            }
        });
        scene.didStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo& info) {
            steps.push_back(RecordStep(info));
        });

        a3d::testing::RunnerTestAccess::start(runner, AtMilliseconds(0));
        runner.pauseSimulation();
        runner.requestSimulationStep();
        runner.requestSimulationStep();
        runner.requestSimulationStep();

        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(0)),
               "resume during a requested step should keep the Runner alive");
        Expect(steps.size() == 1 && !runner.simulationPaused(),
               "resume should finish one requested step and abandon the remainder");
        Expect(LatestFrameStats(runner).simulationStepCount == 1,
               "resume-interrupted request statistics should report one step");

        runner.pauseSimulation();
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(125)),
               "the re-paused remainder check should continue");
        Expect(steps.size() == 1, "resume should clear the unexecuted requested snapshot remainder");

        runner.resumeSimulation();
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(250)),
               "the suppressed post-resume update should continue");
        Expect(steps.size() == 1, "resume should suppress one automatic scheduling opportunity");
        Expect(a3d::testing::RunnerTestAccess::update(runner, AtMilliseconds(375)),
               "the resumed automatic update should continue");
        Expect(steps.size() == 2, "automatic scheduling should resume after one suppression");
        ExpectStep(steps.back(), 1, 0.125, 0.25, 0.125, "the post-request resume automatic step");

        {
            a3d::Scene  repausedScene;
            a3d::Runner repausedRunner(repausedScene, MakeSimulationConfig());
            std::size_t repausedStepCount = 0;

            repausedScene.willStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo&) {
                repausedRunner.resumeSimulation();
            });
            repausedScene.didStepCallback([&](a3d::Scene&, const a3d::Scene::StepInfo&) {
                ++repausedStepCount;
                repausedRunner.pauseSimulation();
            });

            a3d::testing::RunnerTestAccess::start(repausedRunner, AtMilliseconds(1000));
            repausedRunner.pauseSimulation();
            repausedRunner.requestSimulationStep();
            repausedRunner.requestSimulationStep();
            repausedRunner.requestSimulationStep();

            Expect(a3d::testing::RunnerTestAccess::update(repausedRunner, AtMilliseconds(1000)),
                   "resume/re-pause during a requested step should keep the Runner alive");
            Expect(repausedStepCount == 1 && repausedRunner.simulationPaused(),
                   "a resume transition should abandon the old snapshot even after re-pause");
        }
    }

    using TestFunction = void (*)();

    const std::pair<std::string_view, TestFunction>
         Tests[] {{"first-update-zero-delta", FirstUpdateHasZeroDelta},
                  {"elapsed-time-progression", ElapsedTimeProgresses},
                  {"delta-time-progression", DeltaTimeProgresses},
                  {"update-index-progression", UpdateIndexProgresses},
                  {"independent-runner-clocks", RunnerClocksAreIndependent},
                  {"simulation-config-defaults", SimulationConfigDefaults},
                  {"simulation-config-validation", SimulationConfigValidation},
                  {"first-update-zero-simulation-steps", FirstUpdateExecutesNoSimulationSteps},
                  {"simulation-fractional-accumulator", SimulationRetainsFractionalAccumulator},
                  {"simulation-catch-up-overflow", SimulationLimitsCatchUpAndReportsDiscard},
                  {"pause-simulation-preconditions", PauseSimulationPreconditions},
                  {"paused-update-pipeline", PausedUpdatesKeepInputHostAndStatisticsActive},
                  {"pause-clears-fixed-accumulator", PauseClearsFixedAccumulatorWithoutDiscard},
                  {"pause-host-update-boundary", PauseDuringHostUpdateSuppressesScheduling},
                  {"pause-during-catch-up", PauseDuringCatchUpFinishesOnlyCurrentStep},
                  {"resume-boundary", ResumeClearsRequestsAndSuppressesOneAutomaticUpdate},
                  {"requested-step-preconditions", RequestedStepPreconditions},
                  {"requested-step-fixed-execution", RequestedStepsUseFixedDeltaAndIgnoreTimeScale},
                  {"requested-step-command-order", RequestedStepsPreserveCommandAndCallbackOrder},
                  {"requested-step-snapshot-deferral", RequestedStepSnapshotDefersCallbackRequests},
                  {"requested-step-stop-interruption", StopInterruptsRequestedStepsAfterCurrentStep},
                  {"requested-step-resume-interruption", ResumeInterruptsRequestedStepsAndClearsRemainder},
                  {"stop-between-updates", StopBetweenUpdates},
                  {"stop-during-host-update", StopDuringHostUpdate},
                  {"stop-during-runner-callback", StopDuringRunnerCallback},
                  {"stop-during-simulation-step", StopDuringSimulationStep},
                  {"input-before-host-update", InputUpdatesBeforeHostUpdateAndSimulation},
                  {"no-input-context", NoInputContextStillRunsHostUpdate},
                  {"application-runner-access", ApplicationRunnerAccessUsesPreparedRunner},
                  {"simulation-callback-order-physics", SimulationCallbacksRunInOrderWithPhysics},
                  {"scene-command-boundaries", SceneCommandsRespectStepBoundaries},
                  {"render-command-no-valid-render", RenderCommandsWaitWithoutAValidRender},
                  {"render-command-adapter-order", RenderCommandsPrecedeRenderFrameAtAdapterBoundary},
                  {"neither-world-pipeline", NeitherWorldPipeline},
                  {"no-visual-completed-render-count", NoVisualWorldHasZeroCompletedRenderFrames},
                  {"scene-root-invariant", SceneRootInvariant},
                  {"physics-inventory-sampling", PhysicsInventorySamplingIsCurrent},
                  {"physics-inventory-multiple-steps", PhysicsInventoryAcrossMultipleSteps},
                  {"physics-inventory-latest-step", PhysicsInventoryUsesLatestStep},
                  {"physics-inventory-before-did-step", PhysicsInventoryPrecedesDidStep},
                  {"bullet-exact-step", BulletAdvancesExactlyOncePerSimulationStep},
                  {"bullet-invalid-step-delta", BulletRejectsInvalidStepDelta},
                  {"simulation-repeatability", SimulationPhysicsIsRepeatable},
                  {"simulation-120hz-long-run", SimulationAtOneHundredTwentyHertzDoesNotDrift}};

    bool RunTest(std::string_view name, TestFunction function) {

        try {
            function();
            std::cout << "[PASS] " << name << '\n';
            return true;
        }
        catch (const std::exception& exception) {
            std::cerr << "[FAIL] " << name << ": " << exception.what() << '\n';

            return false;
        }
        catch (...) {
            std::cerr << "[FAIL] " << name << ": unknown exception\n";

            return false;
        }
    }

    void PrintAvailableTests() {

        std::cerr << "Available tests:\n";

        for (const auto& [name, function] : Tests) {
            std::cerr << "  " << name << '\n';
        }
    }

}

int main(int argc, char* argv[]) {

    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [test-name]\n";
        PrintAvailableTests();
        return 2;
    }

    // No test name: run the complete suite.

    if (argc == 1) {

        std::size_t passed = 0;
        std::size_t failed = 0;

        for (const auto& [name, function] : Tests) {

            if (RunTest(name, function)) {
                ++passed;
            }
            else {
                ++failed;
            }
        }

        std::cout << '\n' << passed << " passed, " << failed << " failed.\n";

        return failed == 0 ? 0 : 1;
    }

    // One test name: run only that test.
    // CTest continues to use this path.

    const std::string_view requestedTest = argv[1];

    for (const auto& [name, function] : Tests) {

        if (name == requestedTest) {
            return RunTest(name, function) ? 0 : 1;
        }
    }

    std::cerr << "Unknown test: " << requestedTest << '\n';
    PrintAvailableTests();

    return 2;
}
