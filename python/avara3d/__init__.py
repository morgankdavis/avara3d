"""Python interface to Avara3D."""

import sys as _sys

from ._avara3d import (
    AmbientLight,
    Antialiasing,
    Application,
    Box,
    Color,
    Material,
    Mesh,
    Node,
    PointLight,
    RenderContext,
    Runner,
    Scene,
    VisualWorld,
    Window,
    math,
    run,
)

_sys.modules[__name__ + ".math"] = math

__all__ = [
    "AmbientLight",
    "Antialiasing",
    "Application",
    "Box",
    "Color",
    "Material",
    "Mesh",
    "Node",
    "PointLight",
    "RenderContext",
    "Runner",
    "Scene",
    "VisualWorld",
    "Window",
    "run",
]
