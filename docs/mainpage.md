# Avara3D C++ API {#mainpage}

[Back to avara3d.net](https://avara3d.net/)

Avara3D is a C++20 engine for real-time 3D visualization and physical simulation.

This reference documents Avara3D's public API. Two short companion documents provide context that does not belong to any single class:

- [Architecture overview](https://avara3d.net/architecture.html) explains ownership, update and simulation flow, rendering, physics integration, and selected implementation details.
- [Coordinate and Transform Conventions](@ref conventions) records the cross-cutting axis, transform, angle, time, physics-scale, and viewport rules used throughout the API.

## Core entry points

- a3d::Application for application startup, lifecycle hooks, and callback dispatch.
- a3d::Runner and a3d::SimulationConfig for host updates and fixed-step simulation.
- a3d::Scene and a3d::Node for scene ownership and hierarchy.
- a3d::VisualWorld and a3d::PhysicsWorld for visual configuration and rigid-body simulation.

The reference also covers geometry, materials, textures, cameras, lights, input, resource loading, mathematical types and functions, and general utilities.
