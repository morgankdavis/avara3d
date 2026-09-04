# API Reference {#mainpage}

This reference documents Avara3D's public C++ API.

For broader context, the [Architecture Overview](https://avara3d.net/architecture.html) explains high-level ownership, update and simulation flow, rendering, physics integration,
and selected implementation details.

## Key Classes {#key-classes}

- a3d::Application for application startup, lifecycle hooks, and callback dispatch.
- a3d::Runner and a3d::SimulationConfig for host updates and fixed-step simulation.
- a3d::Scene and a3d::Node for scene ownership and hierarchy.
- a3d::VisualWorld and a3d::PhysicsWorld for visual configuration and rigid-body simulation.

The reference also covers geometry, materials, textures, cameras, lights, input, resource loading, mathematical types and functions, and general utilities.

# Conventions {#conventions}

These conventions are global API contracts shared by scene, rendering, physics, input, and mathematical types.

## Axes and orientation

Avara3D uses a right-handed 3D coordinate convention:

- **+X** is right.
- **+Y** is up.
- **-Z** is the conventional forward direction.

A Node's forward(), up(), and right() functions return those local axes after applying the node's orientation. Their
world-space counterparts apply the complete inherited orientation.

## Local, parent, and world transforms

A Node stores position, orientation, and scale relative to its parent.

- Node::transform() maps local coordinates into parent coordinates.
- Node::worldTransform() maps local coordinates into world coordinates.
- Child world transforms are composed as `parentWorld * local`.
- A node's decomposed local transform is composed as translation × rotation × scale.
- Matrices act on column vectors using `matrix * vector`.

The worldPosition(), worldOrientation(), worldScale(), worldForward(), worldUp(), and worldRight() accessors expose
inherited results. convertTo() and convertFrom() move points or transforms between node-local coordinate spaces through world
space.

## Rotations and angles

Public rotation angles are expressed in **radians.**

- Axis-angle rotations store the axis in xyz and the angle in w.
- Euler-angle vectors are ordered as pitch, yaw, and roll.
- Quaternions represent orientation without an angle-unit conversion.

## Time

Runner and Scene lifecycle timing values are expressed in **seconds**:

- Runner::UpdateInfo::elapsedTime and deltaTime use host-time seconds.
- Scene::StepInfo::startTime, endTime, and deltaTime use simulation-time seconds.
- SimulationConfig::timeStep is the fixed simulation-step duration in seconds.

## Scene and physics scale

Geometry and physics values have no inherent units. Applications must use a single internally consistent scale.

The default PhysicsWorld gravity is `(0, -9.807, 0)`, so the built-in physics defaults assume a meter-like scene scale with time
measured in seconds. Extremely small or extremely large world scales can reduce rigid-body stability and collision accuracy.

A PhysicsBody's rigid transform contains translation and orientation. Its local center-of-mass offset is composed when Avara3D
sends a model transform to Bullet and removed when Bullet returns a dynamic-body transform to the owning Node. See
the [architecture overview](https://avara3d.net/architecture.html#physics) for a synchronization diagram.

## Logical viewport coordinates

VisualWorld projection, unprojection, and mesh hit testing use **logical viewport coordinates**:

- X increases to the right.
- Y increases downward from a top-left origin.
- Normalized depth 0 is the near clip plane.
- Normalized depth 1 is the far clip plane.

Framebuffer dimensions may differ from logical viewport dimensions on high-DPI displays; APIs that request logical viewport
coordinates should not be given raw framebuffer pixels.
