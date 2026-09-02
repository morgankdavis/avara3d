//
//  Window.h
//  avara3d
//
//  Created by Morgan Davis on 4/16/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_CONTEXT_WINDOW_H
#define AVARA3D_RENDER_CONTEXT_WINDOW_H

#include <memory>
#include <string>

#include "a3d/render/context/RenderContext.h"

struct GLFWwindow;

namespace a3d {
class Camera;
class Color;
class DesktopInputContext;
class Image;
class Node;
class Renderer;
class Scene;

/**
 * @brief RenderContext for interactive desktop and web rendering.
 *
 * On desktop, Window represents a native application window. On web builds,
 * it represents an HTML canvas backed by WebGL. A3D uses the page's "#canvas"
 * element for browser-specific rendering and input integration.
 */
class Window : public RenderContext {

public:
    // [Public Static Member Functions]

    /** @brief Creates the DesktopInputContext implementation used by Window. */
    static std::unique_ptr<DesktopInputContext> InputContext();

    // [Public Lifecycle Functions]

    /**
     * @brief Creates a rendering window and its rendering resources.
     *
     * For a windowed context, @p size specifies the initial window size. Full-screen
     * windows use the primary monitor's current video mode. High-DPI and antialiasing
     * requests are applied when supported by the platform.
     *
     * @throws std::runtime_error if the window/context initialization fails.
     */
    Window(const math::uvec2& size,
           bool               fullScreen,
           bool               enableHighDPI = true,
           Antialiasing       antialiasing  = Antialiasing::None);

    Window(const Window& other)            = delete;
    Window& operator=(const Window& other) = delete;

    Window(Window&&)            = delete;
    Window& operator=(Window&&) = delete;

    ~Window() override;

    // [Public Member Functions]

    /**
     * @brief Shows the window and begins accepting normal window callbacks.
     *
     * @throws std::runtime_error if the Window has no VisualWorld attached to a Scene.
     */
    void        open();

    /** @brief Closes the window and releases pointer capture. */
    void        close();

    /** @brief Returns whether the Window has been opened and not subsequently closed. */
    bool        isOpen() const;

    /** @brief Returns the window title; web builds currently return an empty string. */
    std::string title() const;

    /** @brief Sets the window title. */
    void        title(const std::string& title);

    /** @brief Returns the window size in platform window coordinates. */
    math::uvec2 size() const;

    /** @brief Sets the window size in platform window coordinates. */
    void        size(const math::uvec2& size);

    /** @brief Returns the window position in screen coordinates. */
    math::uvec2 position() const;

    /** @brief Sets the window position in screen coordinates. */
    void        position(const math::uvec2& pos);

    /** @brief Centers the window on its current monitor when supported by the platform. */
    void        center();

    /** @brief Returns whether the window is explicitly hidden. */
    bool        hidden() const;

    /** @brief Shows or hides the window. */
    void        hidden(bool hidden);

    /** @brief Returns whether pointer input is captured by the window. */
    bool        cursorCaptured() const;

    /**
     * @brief Captures or releases pointer input.
     *
     * Capturing locks pointer motion to the window and hides the pointer. Releasing
     * capture restores the state requested by cursorHidden().
     */
    void        cursorCaptured(bool captured);

    /** @brief Returns whether the pointer is requested to be hidden when not captured. */
    bool        cursorHidden() const;

    /** @brief Shows or hides the pointer when pointer capture is disabled. */
    void        cursorHidden(bool hidden);

    /** @brief Returns whether high-DPI rendering was requested when the Window was created. */
    bool        highDPIEnabled() const;

    // [Public RenderContext Member Functions]

    /** @brief Returns whether vertical synchronization is enabled. */
    bool        vSyncEnabled() const override;

    /**
     * @brief Enables or disables vertical synchronization when supported.
     *
     * Browser presentation timing is controlled by the browser main loop; web
     * builds report VSync enabled and do not support disabling it.
     */
    void        vSyncEnabled(bool enabled) override;

    // [RenderContext Internal Member Functions]

    void        pollEvents() override;

    void        beginFrame(const Scene& scene) override;
    void        endFrame(const Scene& scene) override;

    void        swapBuffers() override;

    math::uvec2 viewportLogicalSize() const override;
    math::uvec2 framebufferSize() const override;

    unsigned    defaultFramebuffer() const override;

    // [Internal Member Functions]

    void        inputContext(DesktopInputContext* inputContext);
    GLFWwindow* glfwWindow() const; // remove?

    // [Internal Static Member Functions]

    static void Destroy(GLFWwindow* window);

private:
    // [Private Static Member Functions]

    static void    GLFWCursorPositionCallback(GLFWwindow* glfwWindow, double xPos, double yPos);
    static void    GLFWMouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods);
    static void    GLFWScrollWheelCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset);
    static void    GLFWKeyCallback(GLFWwindow* glfwWindow, int key, int scanCode, int action, int mods);
    static void    GLFWWindowFocusCallback(GLFWwindow* glfwWindow, int focused);
    static Window* WindowFromGLFWwindow(GLFWwindow* glfwWindow);
    static DesktopInputContext* InputContextFromGLFWWindow(GLFWwindow* glfwWindow);

    // [Private Member Functions]

    void                        registerGLFWCallbacks();
    void                        unregisterGLFWCallbacks();

    // [Private Types]

    struct DestroyGLFWWindow {
        void operator()(GLFWwindow* window) {
            Destroy(window);
        }
    };

    // [Private Member Variables]

    std::unique_ptr<GLFWwindow, DestroyGLFWWindow> _glfwWindow;
    bool                                           _vSyncEnabled;
    bool                                           _cursorCaptured;
    bool                                           _cursorHidden;
    bool                                           _open;
    bool                                           _hidden;
    bool                                           _highDPIEnabled;
    DesktopInputContext*                           _inputContext;
};
} // namespace a3d

#endif // AVARA3D_RENDER_CONTEXT_WINDOW_H
