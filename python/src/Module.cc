//
//  Module.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include <pybind11/pybind11.h>

#include "Bindings.h"

PYBIND11_MODULE(_avara3d, module) {

    module.doc() = "Python bindings for Avara3D";

    auto mathModule = module.def_submodule("math", "Avara3D math types");

    a3d::python::BindMath(mathModule);
    a3d::python::BindColor(module);

    a3d::python::BindRenderContext(module);
    a3d::python::BindWindow(module);

    a3d::python::BindVisualWorld(module);
    a3d::python::BindMaterial(module);
    a3d::python::BindAmbientLight(module);
    a3d::python::BindPointLight(module);

    a3d::python::BindMesh(module);
    a3d::python::BindBox(module);

    a3d::python::BindNode(module);
    a3d::python::BindScene(module);

    a3d::python::BindRunner(module);
    a3d::python::BindApplication(module);
}
