//
//  Window.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include "a3d/Math.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/render/context/Window.h"

namespace py = pybind11;

namespace a3d::python {

void BindWindow(py::module_& module) {

    py::class_<Window, RenderContext, py::smart_holder>(module, "Window")
        .def(py::init<const math::uvec2&,
                      bool,
                      bool,
                      RenderContext::Antialiasing>(),
             py::arg("size"),
             py::arg("fullScreen"),
             py::arg("enableHighDPI") = true,
             py::arg("antialiasing") =
                     RenderContext::Antialiasing::None)
        .def("open", &Window::open)
        .def("close", &Window::close)
        .def("isOpen", &Window::isOpen)
        .def("center", &Window::center);
}

} // namespace a3d::python
