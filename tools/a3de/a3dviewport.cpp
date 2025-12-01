#include "a3dviewport.h"

#include "a3dviewport.h"

#include "a3d/a3d.h"


using namespace a3d;
using namespace a3de;
using namespace std;


//A3DViewport::A3DViewport(RenderingApi renderingApi, a3d::Scene* scene, QWidget* parent):
//        RenderContext(renderingApi),
//        QOpenGLWidget(parent) {
//
//	_scene = scene;
//
//	_width = 800;
//	_height = 600;
//
//    // optional: better default size
//    setMinimumSize(800, 600);
//}

A3DViewport::A3DViewport(RenderingApi renderingApi, QWidget* parent):
		RenderContext(renderingApi),
		QOpenGLWidget(parent) {

	// *** _renderer gets set in RenderContext(), but is null here?? ***
//	_renderer = make_unique<OpenGLRenderer>();

	_width = 800;
	_height = 600;

	//RenderContext::_renderer = make_unique<OpenGLRenderer>();

	// optional: better default size
	setMinimumSize(800, 600);
}

A3DViewport::~A3DViewport() = default;

void A3DViewport::swapBuffers() {} // nada

glm::uvec2 A3DViewport::framebufferSize() const {
	// note that Glfw would expect this without the scale factor
	auto s = devicePixelRatioF();
    return {_width * s, _height * s};
}

glm::vec2 A3DViewport::framebufferScale() const {
	auto s = devicePixelRatioF();
    return {s, s};
}

void A3DViewport::initializeGL() {

	auto loader = [](const char* name) -> void* {
		// QOpenGLContext::getProcAddress returns a function pointer
		// that we can reinterpret_cast to void* for GLAD.
		auto* ctx = QOpenGLContext::currentContext();
		if (!ctx) return nullptr;
		QFunctionPointer fp = ctx->getProcAddress(name);
		return reinterpret_cast<void*>(fp);
	};

	if (a3d::OpenGlRenderer::InitGL(loader)) {
		_renderer->initialize(*this);
	}
	else {
		A3D_LOG_F("!a3d::OpenGlRenderer::InitGL(loader)");
	}
}

void A3DViewport::resizeGL(int w, int h) {
	//A3D_LOG_D("w: {}, h: {}", w, h);
	_width = w;
	_height = h;
}

void A3DViewport::paintGL() {

	if (scene) {
		scene->update_();
	}
}

