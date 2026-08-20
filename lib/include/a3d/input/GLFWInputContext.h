//
//  GLFWInputContext.h
//  avara3d
//
//  Created by Morgan Davis on 4/16/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_INPUT_GLFWINPUTCONTEXT_H
#define AVARA3D_INPUT_GLFWINPUTCONTEXT_H

#include <memory>
#include <set>

#include "a3d/input/DesktopInputContext.h"

struct GLFWwindow;

namespace a3d {

    class Window;

    class GLFWInputContext : public DesktopInputContext {

    public:
        // [Internal Lifecycle Functions]

        GLFWInputContext();
        ~GLFWInputContext() override;

        GLFWInputContext(const InputContext& other)            = delete;
        GLFWInputContext& operator=(const InputContext& other) = delete;

        // [InputContext Internal Member Functions]

        void              attachedToScene(Scene& scene) override;
        void              visualWorldAttachedToScene(Scene& scene) override;

        // [DesktopInputContext Internal Member Functions]

        void              rebaseMouseMotion() override;

        // [Internal Member Functions]

        void              glfwCursorPositionEvent(double xPos, double yPos);
        void              glfwMouseButtonEvent(int button, int action, int mods);
        void              glfwScrollEvent(double xOffset, double yOffset);
        void              glfwKeyEvent(int key, int scanCode, int action, int mods);

        void              detachedFromWindow(Window& window);

    private:
        // [Private Member Functions]

        void    window(Window* window);
        Window* window() const;

        void    initMouseInput();

        // [Private Member Variables]

        Window* _window;
        bool    _hasMousePosition;
    };

}

#endif /* AVARA3D_INPUT_GLFWINPUTCONTEXT_H */
