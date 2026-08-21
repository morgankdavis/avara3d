//
//  GLFWWindow.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/context/Window.h"

#include <iostream>
#include <stdexcept>

#include "a3d/render/backend/opengl/gl.h" // <- MUST be before GLFW
#ifdef A3D_WEB
    #include <emscripten/emscripten.h>
#endif
#include <GLFW/glfw3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include "a3d/log/Log.h"
#include "a3d/input/GLFWInputContext.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/render/backend/opengl/OGLRenderer.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/Filesystem.h"
#include "a3d/visual/VisualWorld.h"

#if defined(A3D_GL_DESKTOP) + defined(A3D_GL_ES) + defined(A3D_GL_WEB) != 1
    #error Exactly one of A3D_GL_DESKTOP, A3D_GL_ES, or A3D_GL_WEB must be defined.
#endif

using namespace a3d;
using namespace a3d::math;
using namespace std;

// [Private Constants]

#if defined(A3D_WEB)
static constexpr const char* WEB_CANVAS_SELECTOR = "#canvas";
#endif

// [Private Static Non-Member Prototypes]

static bool InitGLFW();
#ifdef A3D_WEB
static void InstallWebContextMenuHandler();
static void InstallWebGLContextLostHandler();
#endif
static void GLFWWindowSizeCallback(GLFWwindow* glfwWindow, int width, int height);
static void GLFWWindowCloseCallback(GLFWwindow* glfwWindow);
static void GLFWFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height);
static void GLFWContentScaleCallback(GLFWwindow* glfwWindow, float xScale, float yScale);
static void GLFWErrorCallback(int error, const char* description);
static bool GetGLFWWindowMonitor(GLFWmonitor** monitor, GLFWwindow* window);
static bool GetGLFWMouseMonitor(GLFWmonitor** monitor, GLFWwindow* window);

// [Public Static Member Functions]

unique_ptr<DesktopInputContext> Window::InputContext() {
    return std::make_unique<GLFWInputContext>();
}

// [Public Lifescycle]

Window::Window(const uvec2& size, bool fullScreen, bool enableHighDPI, Antialiasing antialiasing):
    RenderContext {},
    _glfwWindow {},
    _vSyncEnabled {false},
    _cursorCaptured {false},
    _cursorHidden {false},
    _highDPIEnabled {enableHighDPI},
    _open {false},
    _hidden {false},
    _inputContext {} {
    log::d();

    _antialiasing = antialiasing; // see above (?)

    if (InitGLFW()) {
        const string title = util::fs::ExecutableName().value_or("avara3d");

#if defined(A3D_GL_WEB)
        // Emscripten GLFW expects the WebGL version, not the GLES version
        // WebGL 2 corresponds to GLES 3.0 / GLSL ES 300-style shaders
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#elif defined(A3D_GL_ES)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#elif defined(A3D_GL_DESKTOP)
        // TODO: move these version numbers
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // needed for macOS
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, static_cast<int>(antialiasing));
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, (enableHighDPI ? GLFW_TRUE : GLFW_FALSE));
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    #if defined(A3D_LINUX)
            // check if X or Wayland...?
            // TODO: change these
        glfwWindowHintString(GLFW_WAYLAND_APP_ID, title.c_str());
        glfwWindowHintString(GLFW_X11_CLASS_NAME, title.c_str());
        glfwWindowHintString(GLFW_X11_INSTANCE_NAME, title.c_str());
    #endif
    #if defined(A3D_MACOS)
        // the documentation says this has the same effect as GLFW_SCALE_TO_MONITOR, but if you don't also
        // set GLFW_COCOA_RETINA_FRAMEBUFFER to GLFW_FALSE, retina framebuffer isn't actually disabled.
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, (enableHighDPI ? GLFW_TRUE : GLFW_FALSE));
    #endif
#else
    #error No A3D OpenGL backend selected.
#endif

        if (fullScreen) {
            GLFWmonitor*       monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
            _glfwWindow =
                unique_ptr<GLFWwindow, DestroyGLFWWindow>(glfwCreateWindow(vmode->width, vmode->height,
                                                                           title.c_str(), monitor, nullptr));
        }
        else {
            _glfwWindow =
                unique_ptr<GLFWwindow, DestroyGLFWWindow>(glfwCreateWindow((int) size.x, (int) size.y,
                                                                           title.c_str(), nullptr, nullptr));
        }

        if (_glfwWindow) {
            glfwSetWindowUserPointer(_glfwWindow.get(), static_cast<void*>(this));

            glfwMakeContextCurrent(_glfwWindow.get());
            Window::vSyncEnabled(false);

            //if (OGLRenderer::InitGL((GLADloadproc)glfwGetProcAddress)) {
#ifdef A3D_GL_DESKTOP
            if (OGLRenderer::InitGL((GLADloadproc) glfwGetProcAddress)) {
#elif defined(A3D_GL_WEB)
            if (OGLRenderer::InitGL(nullptr)) {
#else
    #error "No GL init path defined for this platform."
#endif
                RenderContext::renderer()->initialize(*this);
                ImGui_ImplGlfw_InitForOpenGL(_glfwWindow.get(), true);
                registerGLFWCallbacks();
            }
            else {
                // TODO: move
                glfwTerminate();
                ImGui_ImplGlfw_Shutdown();
                throw std::runtime_error("Failed to initialize GLAD.");
            }
        }
        else {
            // TODO: move
            glfwTerminate();
            ImGui_ImplGlfw_Shutdown();
            throw std::runtime_error("Couldn't create GLFW Window.");
        }

#ifdef A3D_WEB
        InstallWebContextMenuHandler();
        InstallWebGLContextLostHandler();
#endif
    }

    else {
        throw std::runtime_error("Couldn't initialize GLFW.");
    }
}

Window::~Window() {
    log::d()("Destroying Window {:p}", static_cast<void*>(this));

    inputContext(nullptr);

    if (_glfwWindow) {
        glfwMakeContextCurrent(_glfwWindow.get());

        close();

        // TODO: must modify to support multiple windows
        unregisterGLFWCallbacks();
        ImGui_ImplGlfw_Shutdown();
        _renderer.reset();
        _glfwWindow.reset();
    }

    glfwSetErrorCallback(nullptr);
    glfwTerminate();
}

// [Public Member Functions]

void Window::open() {
    log::i();

// #if defined(A3D_WEB)
// 	log::w()("Window::open() has no effect on web.");
// 	return;
// #else
    if (_visualWorld && _visualWorld->scene()) {
        glfwMakeContextCurrent(_glfwWindow.get());

        glfwSetWindowSizeCallback(_glfwWindow.get(), GLFWWindowSizeCallback);
#if defined(A3D_GL_DESKTOP)
        glfwSetWindowCloseCallback(_glfwWindow.get(), GLFWWindowCloseCallback);
#endif
        glfwSetFramebufferSizeCallback(_glfwWindow.get(), GLFWFramebufferSizeCallback);
        glfwSetWindowContentScaleCallback(_glfwWindow.get(), GLFWContentScaleCallback);

        glfwShowWindow(_glfwWindow.get());

        cursorCaptured(cursorCaptured()); // needs to be set after windows is made current

        _open = true;
    }
    else {
        throw std::runtime_error("Window has no scene.");
    }
// #endif //A3D_WEB
}

void Window::close() {

// #if defined(A3D_WEB)
// 	log::w()("Window::close() has no effect on web.");
// 	return;
// #else
    glfwSetWindowSizeCallback(_glfwWindow.get(), nullptr);
    glfwSetWindowCloseCallback(_glfwWindow.get(), nullptr);
    glfwSetFramebufferSizeCallback(_glfwWindow.get(), nullptr);
    glfwSetWindowContentScaleCallback(_glfwWindow.get(), nullptr);

    cursorCaptured(false);

    glfwSetWindowShouldClose(_glfwWindow.get(), true);

    _open = false;
// #endif //A3D_WEB
}

bool Window::isOpen() const {
    // GLFW_VISIBLE is still true after the window is closed... ?
    // return glfwGetWindowAttrib(_glfwWindow.get(), GLFW_VISIBLE) == GLFW_TRUE;
    return _open;
}

string Window::title() const {
    // return glfwGetWindowTitle(_glfwWindow.get());

#if defined(A3D_WEB)
    return "";
#else
    return glfwGetWindowTitle(_glfwWindow.get());
#endif
}

void Window::title(const string& title) {
    glfwSetWindowTitle(_glfwWindow.get(), title.c_str());
}

uvec2 Window::size() const {
    ivec2 size;
    glfwGetWindowSize(_glfwWindow.get(), &size.x, &size.y);
    return uvec2(size.x, size.y);
}

void Window::size(const uvec2& size) {
    glfwSetWindowSize(_glfwWindow.get(), (int) size.x, (int) size.y);
}

uvec2 Window::position() const {
    ivec2 pos;
    glfwGetWindowPos(_glfwWindow.get(), &pos.x, &pos.y);
    return uvec2(pos.x, pos.y);
}

void Window::position(const uvec2& pos) {
    glfwSetWindowPos(_glfwWindow.get(), (int) pos.x, (int) pos.y);
}

void Window::center() {
#if defined(A3D_GL_DESKTOP)
    // as of GLFW 3.3, there is no "get the monitor this window is on" function.
    // glfwGetWindowMonitor() only applies to full-screen windows.

    GLFWmonitor* monitor = nullptr;
    if (GetGLFWWindowMonitor(&monitor, _glfwWindow.get())) {

        ivec2 screenSize;
        ivec2 screenPos;
        glfwGetMonitorWorkarea(monitor, &screenPos.x, &screenPos.y, &screenSize.x, &screenSize.y);

        auto winSize = this->size();

        this->position(uvec2(screenPos.x + ((screenSize.x / 2.0) - (winSize.x / 2.0)),
                             screenPos.y + ((screenSize.y / 2.0) - (winSize.y / 2.0))));
    }
    else {
        log::e()("Can't get window monitor.");
    }
#else
    log::w()("Window centering is not supported on this platform.");
#endif
}

bool Window::hidden() const {
    // GLFW_VISIBLE seems yp have a mind of its own..
    // return glfwGetWindowAttrib(_glfwWindow.get(), GLFW_VISIBLE) == GLFW_TRUE;
    return _hidden;
}

void Window::hidden(bool hidden) {

    if (hidden) {
        glfwHideWindow(_glfwWindow.get());
    }
    else {
        glfwShowWindow(_glfwWindow.get());
    }
    _hidden = hidden;
}

bool Window::cursorCaptured() const {
    return _cursorCaptured;
}

void Window::cursorCaptured(bool captured) {

    const bool captureChanged = captured != _cursorCaptured;

    if (captured && captureChanged && ImGui::GetCurrentContext()) {
        auto& io = ImGui::GetIO();

        io.ClearEventsQueue();
        io.ClearInputKeys();
        io.ClearInputMouse();
    }

    _cursorCaptured = captured;

    auto window = _glfwWindow.get();

    if (captured) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // GLFW_CURSOR_DISABLED is supposed to hide and lock the cursor,
        // but on at least Wayland + KWin it can remain visible
        static const int     w = 16, h = 16;
        static unsigned char pixels[w * h * 4] = {};
        static GLFWimage     img {w, h, pixels};
        static auto          invCursor = glfwCreateCursor(&img, 0, 0);

        glfwSetCursor(window, invCursor);
    }
    else {
        glfwSetInputMode(window, GLFW_CURSOR, _cursorHidden ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
        glfwSetCursor(window, nullptr);
    }

    if (captureChanged && _inputContext) {
        _inputContext->rebaseMouseMotion();
    }
}

bool Window::cursorHidden() const {
    return _cursorHidden;
}

void Window::cursorHidden(bool hidden) {

    _cursorHidden = hidden;

    if (_cursorCaptured) {
        return;
    }

    glfwSetInputMode(_glfwWindow.get(), GLFW_CURSOR, hidden ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
}

bool Window::highDPIEnabled() const {
    return _highDPIEnabled;
}

// [RenderContext Public Member Functions]

bool Window::vSyncEnabled() const {
    return _vSyncEnabled;
}

void Window::vSyncEnabled(bool enabled) {
#if defined(A3D_GL_DESKTOP) || defined(A3D_GL_ES)
    glfwSwapInterval(enabled ? 1 : 0);
    _vSyncEnabled = enabled;
#elif defined(A3D_GL_WEB)
    // browser presentation timing is controlled by requestAnimationFrame /
    // the Emscripten main loop, not by glfwSwapInterval()
    if (!enabled) {
        log::w()("Vsync is not supported on web.");
    }
    _vSyncEnabled = true;
#else
    log::w()("Swap interval / vsync is not supported on this platform.");
#endif
}

// [RenderContext Internal Member Functions]

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::beginFrame(const Scene& scene) {

    ImGui_ImplGlfw_NewFrame();

    // imgui's GLFW backend manages the native cursor during NewFrame and may
    // restore GLFW_CURSOR_NORMAL. reassert application-requested hiding after
    // imgui has updated its cursor state
    if (_cursorHidden && !_cursorCaptured) {
        glfwSetInputMode(_glfwWindow.get(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}

void Window::endFrame(const Scene& scene) {}

void Window::swapBuffers() {
#if defined(A3D_GL_DESKTOP) || defined(A3D_GL_ES)
    glfwSwapBuffers(_glfwWindow.get());
#endif
}

uvec2 Window::viewportLogicalSize() const {
    // glfwGetWindowSize (what size() uses) return the logical size on Linux and macOS.
    // on Windows and X11 it returns the pixel size (screen coords <-> pixels 1:1)
    // Windows and X11, glfwGetWindowContentScale() will report something other than 1,
    // but it's intended as more of a UI-scaling thing, apparently.
    // see note in RenderContext::viewportScale()
    return size();
}

math::uvec2 Window::framebufferSize() const {
    ivec2 size;
    glfwGetFramebufferSize(_glfwWindow.get(), &size.x, &size.y);
    return uvec2(size.x, size.y);
}

unsigned Window::defaultFramebuffer() const {
    return 0;
}

// [Internal Member Functions]

void Window::inputContext(DesktopInputContext* inputContext) {

    if (_inputContext == inputContext) {
        return;
    }

    auto previousInputContext = dynamic_cast<GLFWInputContext*>(_inputContext);
    _inputContext = inputContext;

    if (previousInputContext) {
        previousInputContext->detachedFromWindow(*this);
    }
}

GLFWwindow* Window::glfwWindow() const {
    return _glfwWindow.get();
}

// [Private Member Functions]

void Window::registerGLFWCallbacks() {

    glfwSetMouseButtonCallback(_glfwWindow.get(), Window::GLFWMouseButtonCallback);
    glfwSetCursorPosCallback(_glfwWindow.get(), Window::GLFWCursorPositionCallback);
    glfwSetScrollCallback(_glfwWindow.get(), Window::GLFWScrollWheelCallback);
    glfwSetKeyCallback(_glfwWindow.get(), Window::GLFWKeyCallback);
    glfwSetWindowFocusCallback(_glfwWindow.get(), Window::GLFWWindowFocusCallback);
}

void Window::unregisterGLFWCallbacks() {

    glfwSetMouseButtonCallback(_glfwWindow.get(), nullptr);
    glfwSetCursorPosCallback(_glfwWindow.get(), nullptr);
    glfwSetScrollCallback(_glfwWindow.get(), nullptr);
    glfwSetKeyCallback(_glfwWindow.get(), nullptr);
    glfwSetWindowFocusCallback(_glfwWindow.get(), nullptr);
}

// [Internal Static Member Functions]

void Window::Destroy(GLFWwindow* window) {
    glfwDestroyWindow(window);
}

// [Private Static member Functions]

void Window::GLFWCursorPositionCallback(GLFWwindow* glfwWindow, double xPos, double yPos) {

#if defined(A3D_WEB)
    // web may css-scale the canvas without resizing the GLFW window.
    // convert browser cursor coordinates back to GLFW logical coordinates.
    int logicalWidth;
    int logicalHeight;
    glfwGetWindowSize(glfwWindow, &logicalWidth, &logicalHeight);

    double cssWidth;
    double cssHeight;

    if (emscripten_get_element_css_size(WEB_CANVAS_SELECTOR, &cssWidth, &cssHeight) == EMSCRIPTEN_RESULT_SUCCESS
        && cssWidth > 0.0 && cssHeight > 0.0) {
        xPos *= static_cast<double>(logicalWidth) / cssWidth;
        yPos *= static_cast<double>(logicalHeight) / cssHeight;
    }
#endif

    auto window = WindowFromGLFWwindow(glfwWindow);
    auto inputContext = window->_inputContext;

    if (inputContext) {
        static_cast<GLFWInputContext*>(inputContext)->glfwCursorPositionEvent(xPos, yPos);
    }

    if (!window->cursorCaptured()) {
        ImGui_ImplGlfw_CursorPosCallback(glfwWindow, xPos, yPos);
    }
}

void Window::GLFWMouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {

    auto window = WindowFromGLFWwindow(glfwWindow);
    auto inputContext = static_cast<GLFWInputContext*>(window->_inputContext);

    if (!window->cursorCaptured()) {
        ImGui_ImplGlfw_MouseButtonCallback(glfwWindow, button, action, mods);
    }

    if (!inputContext) {
        return;
    }

    if (window->cursorCaptured()) {
        inputContext->glfwMouseButtonEvent(button, action, mods);
        return;
    }

    const auto a3dButton = static_cast<DesktopInputContext::MouseButton>(button);

    if (action == GLFW_PRESS) {

        if (!ImGui::GetIO().WantCaptureMouse) {
            inputContext->glfwMouseButtonEvent(button, action, mods);
        }
    }
    else if (action == GLFW_RELEASE) {

        // if A3D saw the press, it must also see the release even if
        // ImGui has captured the mouse in the meantime
        if (inputContext->mouseButtonDown(a3dButton)) {
            inputContext->glfwMouseButtonEvent(button, action, mods);
        }
    }
}

void Window::GLFWScrollWheelCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset) {

    auto window = WindowFromGLFWwindow(glfwWindow);
    auto inputContext = static_cast<GLFWInputContext*>(window->_inputContext);

    if (!window->cursorCaptured()) {
        ImGui_ImplGlfw_ScrollCallback(glfwWindow, xOffset, yOffset);
    }

    if (!inputContext) {
        return;
    }

    if (window->cursorCaptured() || !ImGui::GetIO().WantCaptureMouse) {
        inputContext->glfwScrollEvent(xOffset, yOffset);
    }
}

void Window::GLFWKeyCallback(GLFWwindow* glfwWindow, int key, int scanCode, int action, int mods) {

    auto window = WindowFromGLFWwindow(glfwWindow);
    auto inputContext = static_cast<GLFWInputContext*>(window->_inputContext);

    if (!window->cursorCaptured()) {
        ImGui_ImplGlfw_KeyCallback(glfwWindow, key, scanCode, action, mods);
    }

    if (!inputContext) {
        return;
    }

    if (window->cursorCaptured()) {
        inputContext->glfwKeyEvent(key, scanCode, action, mods);
        return;
    }

    const auto a3dKey = static_cast<DesktopInputContext::Key>(key);

    if (action == GLFW_PRESS) {

        if (!ImGui::GetIO().WantCaptureKeyboard) {
            inputContext->glfwKeyEvent(key, scanCode, action, mods);
        }
    }
    else if (action == GLFW_RELEASE) {

        // if A3D saw the press, it must also see the release even if
        // ImGui has captured the keyboard in the meantime
        if (inputContext->keyDown(a3dKey)) {
            inputContext->glfwKeyEvent(key, scanCode, action, mods);
        }
    }
}

void Window::GLFWWindowFocusCallback(GLFWwindow* glfwWindow, int focused) {

    ImGui_ImplGlfw_WindowFocusCallback(glfwWindow, focused);

    if (focused == GLFW_TRUE) {
        return;
    }

    auto window = WindowFromGLFWwindow(glfwWindow);

    if (window->_inputContext) {
        window->_inputContext->releaseAllInputs();
    }
}

Window* Window::WindowFromGLFWwindow(GLFWwindow* glfwWindow) {
    return (Window*) glfwGetWindowUserPointer(glfwWindow);
}

DesktopInputContext* Window::InputContextFromGLFWWindow(GLFWwindow* glfwWindow) {
    return WindowFromGLFWwindow(glfwWindow)->_inputContext;
}

// [Private Static Non-Member Functions]

bool InitGLFW() {

    static bool initialized = false;
    if (!initialized) {
        log::i();

        int glfwMajVers, glfwMinVers, glfwRev;
        glfwGetVersion(&glfwMajVers, &glfwMinVers, &glfwRev);
        log::i()("Starting GLFW version {}.{}.{}...", glfwMajVers, glfwMinVers, glfwRev);

        // TODO: must modify to support multiple windows
        glfwSetErrorCallback(GLFWErrorCallback);

        if (glfwInit()) {
            log::i()("GLFW Initialized.");
        }
        else {
            log::f()("Error initializing GLFW.");
            return false;
        }

        srand(time(nullptr)); // where else can we put this?

        initialized = true;
    }
    return true;
}

#ifdef A3D_WEB
void InstallWebContextMenuHandler() {
    // suppressed right-click in Emscripten canvas
    auto result = emscripten_set_contextmenu_callback(WEB_CANVAS_SELECTOR, nullptr, false,
                                                      [](int, const EmscriptenMouseEvent*, void*) -> EM_BOOL {
                                                          return EM_TRUE;
                                                      });

    if (result != EMSCRIPTEN_RESULT_SUCCESS) {
        log::e()("Error setting Emscripten context menu callback: {}", result);
    }
}

void InstallWebGLContextLostHandler() {
    const auto result = emscripten_set_webglcontextlost_callback(WEB_CANVAS_SELECTOR, nullptr, false,
                                                                 [](int, const void*, void*) -> EM_BOOL {
                                                                     log::e()("WebGL context lost.");
                                                                     return EM_FALSE;
                                                                 });

    if (result != EMSCRIPTEN_RESULT_SUCCESS) {
        log::e()("Error registering WebGL context-lost callback: {}", result);
    }
}

#endif

void GLFWWindowSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
//	log::d()("glfwWindow: {:p}, width: {}, height: {}",
//			  static_cast<void*>(glfwWindow), width, height);

    auto window = (Window*) glfwGetWindowUserPointer(glfwWindow);
    window->size(uvec2(width, height));
}

void GLFWWindowCloseCallback(GLFWwindow* glfwWindow) {
    log::i()("glfwWindow: {:p}", static_cast<void*>(glfwWindow));

    auto window = (Window*) glfwGetWindowUserPointer(glfwWindow);
    window->close();
}

void GLFWFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
//	log::d()("glfwWindow: {:p}, width: {}, height: {}",
//			  static_cast<void*>(glfwWindow), width, height);

//	auto window = (GLFWWindow*)glfwGetWindowUserPointer(glfwWindow);
//	window->renderer()->viewportScaleChanged(*window);
}

void GLFWContentScaleCallback(GLFWwindow* glfwWindow, float xScale, float yScale) {
    log::d()("glfwWindow: {:p}, xScale: {}, yScale: {}", static_cast<void*>(glfwWindow), xScale, yScale);
}

void GLFWErrorCallback(int error, const char* description) {
    log::e()("error: {}, description: {}", error, description);
}

bool GetGLFWWindowMonitor(GLFWmonitor** monitor, GLFWwindow* window) {
    // https://github.com/glfw/glfw/issues/1699#issuecomment-723692566

    bool success = false;

    int windowRect[4] = {0};
    glfwGetWindowPos(window, &windowRect[0], &windowRect[1]);
    glfwGetWindowSize(window, &windowRect[2], &windowRect[3]);

    int           monitorsSize = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorsSize);

    GLFWmonitor* closestMonitor = NULL;
    int          maxOverlapArea = 0;

    for (int i = 0; i < monitorsSize; ++i) {

        int monitorPos[2] = {0};
        glfwGetMonitorPos(monitors[i], &monitorPos[0], &monitorPos[1]);

        const GLFWvidmode* monitorVideoMode = glfwGetVideoMode(monitors[i]);

        // https://github.com/glfw/glfw/issues/1699#issuecomment-1892387147
        int monitorRect[4] = {
            monitorPos[0],
            monitorPos[1],
            monitorVideoMode->width,
            monitorVideoMode->height,
        };

        if (!(((windowRect[0] + windowRect[2]) < monitorRect[0])
              || (windowRect[0] > (monitorRect[0] + monitorRect[2]))
              || ((windowRect[1] + windowRect[3]) < monitorRect[1])
              || (windowRect[1] > (monitorRect[1] + monitorRect[3])))) {

            int intersectionRect[4] = {0};

            // x, width
            if (windowRect[0] < monitorRect[0]) {
                intersectionRect[0] = monitorRect[0];

                if ((windowRect[0] + windowRect[2]) < (monitorRect[0] + monitorRect[2])) {
                    intersectionRect[2] = (windowRect[0] + windowRect[2]) - intersectionRect[0];
                }
                else {
                    intersectionRect[2] = monitorRect[2];
                }
            }
            else {
                intersectionRect[0] = windowRect[0];

                if ((monitorRect[0] + monitorRect[2]) < (windowRect[0] + windowRect[2])) {
                    intersectionRect[2] = (monitorRect[0] + monitorRect[2]) - intersectionRect[0];
                }
                else {
                    intersectionRect[2] = windowRect[2];
                }
            }

            // y, height
            if (windowRect[1] < monitorRect[1]) {
                intersectionRect[1] = monitorRect[1];

                if ((windowRect[1] + windowRect[3]) < (monitorRect[1] + monitorRect[3])) {
                    intersectionRect[3] = (windowRect[1] + windowRect[3]) - intersectionRect[1];
                }
                else {
                    intersectionRect[3] = monitorRect[3];
                }
            }
            else {
                intersectionRect[1] = windowRect[1];

                if ((monitorRect[1] + monitorRect[3]) < (windowRect[1] + windowRect[3])) {
                    intersectionRect[3] = (monitorRect[1] + monitorRect[3]) - intersectionRect[1];
                }
                else {
                    intersectionRect[3] = windowRect[3];
                }
            }

            // https://github.com/glfw/glfw/issues/1699#issuecomment-1892387147
            //int overlap_area = intersection_rectangle[3] * intersection_rectangle[4];
            int overlapArea = intersectionRect[2] * intersectionRect[3];
            if (overlapArea > maxOverlapArea) {
                closestMonitor = monitors[i];
                maxOverlapArea = overlapArea;
            }
        }
    }

    if (closestMonitor) {
        *monitor = closestMonitor;
        success = true;
    }

    // true: monitor contains the monitor the window is most on
    // false: monitor is unmodified
    return success;
}

bool GetGLFWMouseMonitor(GLFWmonitor** monitor, GLFWwindow* window) {
    // https://github.com/glfw/glfw/issues/1699#issuecomment-723692566

    bool success = false;

    double cursorPos[2] = {0};
    glfwGetCursorPos(window, &cursorPos[0], &cursorPos[1]);

    int windowPos[2] = {0};
    glfwGetWindowPos(window, &windowPos[0], &windowPos[1]);

    int           monitorsSize = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorsSize);

    // convert cursor position from window coordinates to screen coordinates
    cursorPos[0] += windowPos[0];
    cursorPos[1] += windowPos[1];

    for (int i = 0; ((!success) && (i < monitorsSize)); ++i) {

        int monitorPos[2] = {0};
        glfwGetMonitorPos(monitors[i], &monitorPos[0], &monitorPos[1]);

        const GLFWvidmode* monitorVideoMode = glfwGetVideoMode(monitors[i]);

        if ((cursorPos[0] < monitorPos[0]) || (cursorPos[0] > (monitorPos[0] + monitorVideoMode->width))
            || (cursorPos[1] < monitorPos[1]) || (cursorPos[1] > (monitorPos[1] + monitorVideoMode->height))) {

            *monitor = monitors[i];
            success = true;
        }
    }

    // true: monitor contains the monitor the mouse is on
    // false: monitor is unmodified
    return success;
}
