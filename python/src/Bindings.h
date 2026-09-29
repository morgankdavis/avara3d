//
//  Bindings.h
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PYTHON_BINDINGS_H
#define AVARA3D_PYTHON_BINDINGS_H

#include <pybind11/pybind11.h>

namespace a3d::python {

void BindMath(pybind11::module_& module);
void BindColor(pybind11::module_& module);

void BindRenderContext(pybind11::module_& module);
void BindWindow(pybind11::module_& module);

void BindVisualWorld(pybind11::module_& module);
void BindMaterial(pybind11::module_& module);
void BindAmbientLight(pybind11::module_& module);
void BindPointLight(pybind11::module_& module);

void BindMesh(pybind11::module_& module);
void BindBox(pybind11::module_& module);

void BindNode(pybind11::module_& module);
void BindScene(pybind11::module_& module);

void BindApplication(pybind11::module_& module);
void BindRunner(pybind11::module_& module);

} // namespace a3d::python

#endif // AVARA3D_PYTHON_BINDINGS_H
