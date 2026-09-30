#include <stdexcept>
#include <string>

#include <pybind11/pybind11.h>

#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>
#include <GLFW/emscripten_glfw3.h>

namespace py = pybind11;

namespace {

GLFWwindow* Window = nullptr;

std::string glfwVersion() {
	return glfwGetVersionString();
}

void drawBlueFrame() {

	if (!Window) {

		if (!glfwInit()) {
			throw std::runtime_error("glfwInit() failed");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		emscripten::glfw3::SetNextWindowCanvasSelector("#canvas");

		Window = glfwCreateWindow(
				640,
				480,
				"Pyodide + GLFW",
				nullptr,
				nullptr);

		if (!Window) {
			glfwTerminate();
			throw std::runtime_error("glfwCreateWindow() failed");
		}
	}

	glfwMakeContextCurrent(Window);

	int width;
	int height;
	glfwGetFramebufferSize(Window, &width, &height);

	glViewport(0, 0, width, height);
	glClearColor(0.0F, 0.0F, 1.0F, 1.0F);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwSwapBuffers(Window);
}

} // namespace

PYBIND11_MODULE(_glfw_poc, module) {

	module.doc() = "Pyodide + GLFW proof of concept";

	module.def("glfw_version", &glfwVersion);
	module.def("draw_blue_frame", &drawBlueFrame);
}
