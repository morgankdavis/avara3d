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
#include "a3d/scene/Scene.h"
#include "a3d/util/Timer.h"
#include "a3d/visual/VisualWorld.h"
#include "log/Log.h"

namespace a3d {

/**
 * @brief Base class and top-level entry point for A3D applications.
 *
 * Applications normally derive from Application, implement init() to create
 * the initial Scene, and optionally override the protected lifecycle and
 * simulation callbacks. Transfer ownership of the derived instance to Run(),
 * which drives the platform host loop for the lifetime of the application.
 *
 * Application owns the Scene and Runner created for an application and
 * coordinates initialization, host updates, input, simulation callbacks,
 * rendering, contact callbacks, and shutdown.
 *
 * Unlike other A3D classes, Application is intentionally designed to be
 * subclassed by application code; its protected API is part of the supported
 * application interface.
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
     * @throws std::invalid_argument if @p application is nullptr or its initial
     * SimulationConfig is invalid.
     * @throws std::runtime_error if Application::init() returns nullptr.
     */
    static int Run(std::unique_ptr<Application> application);

    // [Public Lifecycle Functions]

    /**
     * @brief Creates application state from process arguments and configures logging.
     *
     * Derived application constructors normally forward their process arguments
     * and desired initial log level to this constructor.
     *
     * @param argc number of process command-line arguments.
     * @param argv process command-line argument array.
     * @param logLevel initial application log level.
     */
    Application(int argc, char* argv[], log::Level logLevel = log::Level::Info);

    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;

    Application(Application&&)            = delete;
    Application& operator=(Application&&) = delete;

    virtual ~Application();

protected:
    // [Protected Types]

    /** @brief Callable queued for execution at a simulation-step boundary. */
    using SceneCommand = std::function<void(Scene&)>;

    // [Protected Member Functions]

    /**
     * @brief Creates the initial Scene for the application.
     *
     * Called once during startup before the Runner is created. Implementations
     * must return a non-null Scene. runner() and scene() are not available while
     * init() is executing.
     *
     * @return the Scene to own and drive for the lifetime of the application.
     */
    virtual std::unique_ptr<Scene>  init() = 0;

    /**
     * @brief Supplies the initial simulation scheduling configuration.
     *
     * Called once after init() and before the Runner is started. The default
     * implementation returns a default-constructed SimulationConfig.
     */
    virtual SimulationConfig        simulationConfig() const;

    /**
     * @brief Determines whether the application host loop should continue.
     *
     * Called before each Runner host update. Returning false stops the Runner
     * and begins application shutdown. The default implementation returns true.
     */
    virtual bool                    shouldContinue(const Scene& scene);

    /**
     * @brief Called after the Runner and Scene have been destroyed during shutdown.
     *
     * The default implementation does nothing. Exceptions escaping this hook
     * are ignored because application teardown must not throw.
     */
    virtual void                    didShutdown();

    /**
     * @brief Returns the application's Runner after initialization.
     *
     * @throws std::logic_error if the Runner has not yet been initialized.
     */
    Runner&                         runner();

    /**
     * @brief Returns the application's Runner after initialization.
     *
     * @throws std::logic_error if the Runner has not yet been initialized.
     */
    const Runner&                   runner() const;

    /**
     * @brief Returns the application's Scene after initialization.
     *
     * @throws std::logic_error if the Scene has not yet been initialized.
     */
    Scene&                          scene();

    /**
     * @brief Returns the application's Scene after initialization.
     *
     * @throws std::logic_error if the Scene has not yet been initialized.
     */
    const Scene&                    scene() const;

    /**
     * @brief Queues @p command to run before the next simulation step.
     *
     * The command runs before sceneWillStep() and before physics advances.
     *
     * @throws std::invalid_argument if @p command is empty.
     */
    void                            queueScenePreStepCommand(SceneCommand command);

    /**
     * @brief Queues @p command to run after the next simulation step.
     *
     * The command runs after physics advances and before sceneDidStep().
     *
     * @throws std::invalid_argument if @p command is empty.
     */
    void                            queueScenePostStepCommand(SceneCommand command);

    /** @brief Returns process command-line arguments excluding the executable name. */
    const std::vector<std::string>& args() const;

    /**
     * @brief Called near the beginning of each Runner host update.
     *
     * This hook runs before event polling, input update, simulation scheduling,
     * and rendering. The default implementation does nothing.
     */
    virtual void                    runnerUpdate(Runner& runner, Scene& scene, const Runner::UpdateInfo& info);

    /**
     * @brief Called after the InputContext has processed the current host update.
     *
     * The default implementation does nothing.
     */
    virtual void                    inputDidUpdate(Runner&       runner,
                                                   Scene&        scene,
                                                   InputContext& inputContext,
                                                   const InputContext::UpdateInfo&);

    /**
     * @brief Called immediately before each Scene simulation step.
     *
     * Queued pre-step Scene commands execute before this hook. The default
     * implementation does nothing.
     */
    virtual void                    sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info);

    /**
     * @brief Called immediately after each Scene simulation step.
     *
     * Queued post-step Scene commands execute before this hook. The default
     * implementation does nothing.
     */
    virtual void                    sceneDidStep(Runner& runner, Scene& scene, const Scene::StepInfo& info);

    /**
     * @brief Called after a render frame begins and before scene traversal and drawing.
     *
     * The default implementation does nothing.
     */
    virtual void                    frameDidBegin(Runner&                        runner,
                                                  Scene&                         scene,
                                                  VisualWorld&                   visualWorld,
                                                  const VisualWorld::RenderInfo& info);

    /**
     * @brief Called when the PhysicsWorld reports the beginning of a contact.
     *
     * The default implementation does nothing.
     */
    virtual void                    contactDidBegin(Runner&               runner,
                                                    Scene&                scene,
                                                    PhysicsWorld&         physicsWorld,
                                                    const PhysicsContact& contact);

    /**
     * @brief Called when the PhysicsWorld reports a continuing contact.
     *
     * The default implementation does nothing.
     */
    virtual void                    contactDidContinue(Runner&               runner,
                                                       Scene&                scene,
                                                       PhysicsWorld&         physicsWorld,
                                                       const PhysicsContact& contact);

    /**
     * @brief Called when the PhysicsWorld reports the end of a contact.
     *
     * The default implementation does nothing.
     */
    virtual void                    contactDidEnd(Runner&               runner,
                                                  Scene&                scene,
                                                  PhysicsWorld&         physicsWorld,
                                                  const PhysicsContact& contact);

private:
    // [Private Member Functions]

    void initLog(log::Level level);
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
    util::Timer               _startupTimer;
};

} // namespace a3d

#endif // AVARA3D_APPLICATION_H
