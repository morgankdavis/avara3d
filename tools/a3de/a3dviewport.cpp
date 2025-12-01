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
    //return {_width, _height};
	return {800, 600};
}

glm::vec2 A3DViewport::framebufferScale() const {
    return {1.0, 1.0};
}

void A3DViewport::initializeGL() {

	//GlfwWindow::InitGLAD(); // a3de




	auto loader = [](const char* name) -> void* {
		// QOpenGLContext::getProcAddress returns a function pointer
		// that we can reinterpret_cast to void* for GLAD.
		auto* ctx = QOpenGLContext::currentContext();
		if (!ctx) return nullptr;
		QFunctionPointer fp = ctx->getProcAddress(name);
		return reinterpret_cast<void*>(fp);
	};

	if (!a3d::GlfwWindow::Init(loader)) {
		// log + maybe throw or mark an error flag
		A3D_LOG_E("!a3d::GlfwWindow::Init(loader)");
	}

	A3D_LOG_I("viewport member renderer1: {}", (void*)this->_renderer.get());
	A3D_LOG_I("base RenderContext renderer1: {}", (void*)RenderContext::renderer());

//	if (this->_renderer == nullptr) {
//		this->_renderer = make_unique<OpenGLRenderer>();
	_renderer->initialize(*this);
//	}

//	A3D_LOG_I("viewport member renderer2: {}", (void*)this->_renderer.get());
//	A3D_LOG_I("base RenderContext renderer2: {}", (void*)RenderContext::renderer());



	const GLubyte* vendor   = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version  = glGetString(GL_VERSION);

	//RenderContext::_renderer->initialize(*this);
	//this->_renderer->initialize(*this);

	A3D_LOG_I("GL_VENDOR  : {}", vendor   ? reinterpret_cast<const char*>(vendor)   : "<null>");
	A3D_LOG_I("GL_RENDERER: {}", renderer ? reinterpret_cast<const char*>(renderer) : "<null>");
	A3D_LOG_I("GL_VERSION : {}", version  ? reinterpret_cast<const char*>(version)  : "<null>");



//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//	glEnable(GL_DEPTH_TEST);

//	A3D_LOG_I("viewport member renderer3: {}", (void*)this->_renderer.get());
//	A3D_LOG_I("base RenderContext renderer3: {}", (void*)RenderContext::renderer());

	A3D_LOG_I("nada");
}

void A3DViewport::resizeGL(int w, int h) {
	A3D_LOG_D("w: {}, h: {}", w, h);

    // if (m_engine) {
    //     m_engine->resize(w, h);
    // }
	_width = w;
	_height = h;
}

void A3DViewport::paintGL() {

	//glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

	if (scene) {
		scene->update_();
	}

	//glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    // run one drame in a3d.
    // _scene->update();

    // then, immediately ask Qt to do it again.
    //update();

    // if (m_engine) {
    //     m_engine->renderFrame();
    // }
}

