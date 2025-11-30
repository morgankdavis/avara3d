#include "a3dviewport.h"

#include "a3dviewport.h"

#include "a3d/a3d.h"


using namespace a3d;
using namespace a3de;
using namespace std;


A3DViewport::A3DViewport(RenderingApi renderingApi, QWidget* parent):
        RenderContext(renderingApi),
        QOpenGLWidget(parent) {
    // optional: better default size
    setMinimumSize(800, 600);
}

A3DViewport::~A3DViewport() = default;

void A3DViewport::swapBuffers() {

    // nada
}

glm::uvec2 A3DViewport::framebufferSize() const {
    return {320, 240};
}

glm::vec2 A3DViewport::framebufferScale() const {
    return {1.0, 1.0};
}

void A3DViewport::initializeGL() {

    //initializeOpenGLFunctions();

           // create/configure your engine against the current GL context
    //m_engine = std::make_unique<a3d::Engine>();

           // e.g.
           // m_engine->initialize(/*maybe pass function loader*/);
}

void A3DViewport::resizeGL(int w, int h) {

    // if (m_engine) {
    //     m_engine->resize(w, h);
    // }
}

void A3DViewport::paintGL() {

    // run one drame in a3d.
    // _scene->update();

    // then, immediately ask Qt to do it again.
    update();

    // if (m_engine) {
    //     m_engine->renderFrame();
    // }
}

