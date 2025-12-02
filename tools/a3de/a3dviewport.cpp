#include "a3dviewport.h"

#include <QDateTime>

#include "imgui.h"

#include "a3d/a3d.h"


using namespace a3d;
using namespace a3de;
using namespace std;


/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

A3DViewport::A3DViewport(RenderingApi renderingApi, QWidget* parent):
		RenderContext(renderingApi),
		QOpenGLWidget(parent),
		_scene{} {

	// optional: better default size
	setMinimumSize(1280, 768);
}

A3DViewport::~A3DViewport() {

	ImGui::DestroyContext();
}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

Scene* A3DViewport::scene() const {
	return _scene;
}

void A3DViewport::scene(Scene* scene) {
	_scene = scene;
}

/*********************************************************************************************
	QOpenGLWidget Protected Member Functions
 *********************************************************************************************/

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

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
		ImGui::StyleColorsDark();
		io.DisplaySize = ImVec2(float(width()), float(height()));

		_renderer->initialize(*this);
	}
	else {
		A3D_LOG_F("Failed to initialize OpenGL function loader.");
	}
}

void A3DViewport::resizeGL(int w, int h) {}

void A3DViewport::paintGL() {

	static qint64 lastNs = 0;
	qint64 nowNs = QDateTime::currentMSecsSinceEpoch() * 1000000ll;
	if (lastNs == 0) lastNs = nowNs;
	double dt = double(nowNs - lastNs) / 1e9;
	lastNs = nowNs;

	ImGuiIO& io = ImGui::GetIO();

	const float w  = float(width());
	const float h  = float(height());
	const float dpr = float(devicePixelRatioF());

	io.DisplaySize = ImVec2(w, h);
	io.DisplayFramebufferScale = ImVec2(dpr, dpr);

	io.DeltaTime = (dt > 0.0) ? float(dt) : 1.0f/60.0f;

	_scene->update();

	update();
}

/*********************************************************************************************
	RenderContext Public Member Functions
 *********************************************************************************************/

bool A3DViewport::vSyncEnabled() const {
	return true;
}

void A3DViewport::vSyncEnabled(bool enabled) {
	throw Exception("Qt forces vSync.");
}

/*********************************************************************************************
	RenderContext Internal Member Functions
 *********************************************************************************************/

void A3DViewport::beginFrame(const a3d::Scene& scene) {}

void A3DViewport::endFrame(const a3d::Scene& scene) {}

void A3DViewport::swapBuffers() {} // nada

glm::uvec2 A3DViewport::framebufferSize() const {
	// note that Glfw handles scale a little differently and
	// would expect this without the scale factor.
	auto s = devicePixelRatioF();
	return {width() * s, height() * s};
}

glm::vec2 A3DViewport::framebufferScale() const {
	auto s = devicePixelRatioF();
    return {s, s};
}

unsigned A3DViewport::defaultFramebuffer() const {
	return static_cast<unsigned>(defaultFramebufferObject());
}
