//
//  ae.h
//  avara-engine
//
//  Created by Morgan Davis on 9/20/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef ae_h
#define ae_h


#include "ae/Buffer.h"
#include "ae/Color.h"
#include "ae/CubeImage.h"
#include "ae/Font.h"
#include "ae/Global.h"
#include "ae/Image.h"
#include "ae/Types.h"
#include "ae/diagnostic/Exception.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/diagnostic/logging/sinks/FileLoggerSink.h"
#if defined(ANDROID)
#include "diagnostic/logging/sinks/platform/android/FileLoggerSink.h"
#elif defined(DESKTOP)
#include "ae/diagnostic/logging/sinks/platform/desktop/StdOutLoggerSink.h"
#endif
#include "ae/geometry/Geometry.h"
#include "ae/geometry/GeometryElement.h"
#include "ae/geometry/Line.h"
#include "ae/geometry/Point.h"
#include "ae/geometry/primitives/Box.h"
#include "ae/geometry/primitives/Capsule.h"
#include "ae/geometry/primitives/Cone.h"
#include "ae/geometry/primitives/Cylinder.h"
#include "ae/geometry/primitives/Plane.h"
#include "ae/geometry/primitives/Pyramid.h"
#include "ae/geometry/primitives/Sphere.h"
#include "ae/geometry/primitives/Torus.h"
#include "ae/geometry/primitives/Tube.h"
#include "ae/input/InputManager.h"
#include "ae/input/platform/desktop/WindowInputManager.h" // remove?
#include "ae/physics/HitTestResult.h"
#include "ae/physics/PhysicsBody.h"
#include "ae/physics/PhysicsContact.h"
#include "ae/physics/PhysicsShape.h"
#include "ae/physics/PhysicalWorld.h"
#include "ae/physics/shape_primitives/BoxPhysicsShape.h"
#include "ae/physics/shape_primitives/CapsulePhysicsShape.h"
#include "ae/physics/shape_primitives/ConePhysicsShape.h"
#include "ae/physics/shape_primitives/CylinderPhysicsShape.h"
#include "ae/physics/shape_primitives/PlanePhysicsShape.h"
#include "ae/physics/shape_primitives/SpherePhysicsShape.h"
#include "ae/rendering/Light.h"
#include "ae/rendering/Renderer.h"
#include "ae/rendering/VisualWorld.h"
#include "ae/rendering/camera/Camera.h"
#include "ae/rendering/camera/OrthographicCamera.h"
#include "ae/rendering/camera/PerspectiveCamera.h"
#include "ae/rendering/context/RenderContext.h"
#include "ae/rendering/context/platform/desktop/Window.h"
#include "ae/rendering/materials/Material.h"
#include "ae/rendering/materials/MaterialProperty.h"
#include "ae/rendering/materials/MaterialPropertyContents.h"
#include "ae/rendering/opengl/Program.h"
#include "ae/scene/Node.h"
#include "ae/scene/Scene.h"


#endif /* ae_h */
