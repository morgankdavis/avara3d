#include "a3dviewport.h"

#include "a3d/a3d.h"


using namespace a3d;
using namespace a3de;
using namespace std;


A3DViewport::A3DViewport(RenderingApi renderingApi, QWidget* parent):
		RenderContext(renderingApi),
		QOpenGLWidget(parent) {

	// optional: better default size
	setMinimumSize(1280, 768);
}

A3DViewport::~A3DViewport() = default;

void A3DViewport::beginFrame(const a3d::Scene& scene) {

}

void A3DViewport::endFrame(const a3d::Scene& scene) {

}

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

unsigned A3DViewport::defaultFramebuffer() const {
	return static_cast<unsigned>(defaultFramebufferObject());
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
	_width = w;
	_height = h;
}

void A3DViewport::paintGL() {

	scene->update();
	update();
}

