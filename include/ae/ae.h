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
#include "ae/BuildInfo.h"
#include "ae/Color.h"
#include "ae/Configuration.h"
#include "ae/CubeImage.h"
#include "ae/Font.h"
#include "ae/Image.h"
#include "ae/Types.h"
#include "ae/diagnostic/exception/Exception.h"
#include "ae/diagnostic/exception/UnsupportedFormat.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/diagnostic/logging/sink/FileLoggerSink.h"
#if defined(ANDROID)
#include "diagnostic/logging/sink/platform/android/FileLoggerSink.h"
#elif defined(DESKTOP)
#include "ae/diagnostic/logging/sink/platform/desktop/StdOutLoggerSink.h"
#endif

#include "ae/input/InputManager.h"
#include "ae/input/platform/desktop/WindowInputManager.h" // remove?
#include "ae/mesh/Mesh.h"
#include "ae/mesh/MeshElement.h"
#include "ae/mesh/Line.h"
#include "ae/mesh/Point.h"
#include "ae/mesh/primitive/Box.h"
#include "ae/mesh/primitive/Capsule.h"
#include "ae/mesh/primitive/Cone.h"
#include "ae/mesh/primitive/Cylinder.h"
#include "ae/mesh/primitive/Plane.h"
#include "ae/mesh/primitive/Pyramid.h"
#include "ae/mesh/primitive/Sphere.h"
#include "ae/mesh/primitive/Torus.h"
#include "ae/mesh/primitive/Tube.h"
#include "ae/physics/HitTestResult.h"
#include "ae/physics/PhysicsBody.h"
#include "ae/physics/PhysicsContact.h"
#include "ae/physics/PhysicsShape.h"
#include "ae/physics/PhysicalWorld.h"
#include "ae/physics/shape_primitive/BoxPhysicsShape.h"
#include "ae/physics/shape_primitive/CapsulePhysicsShape.h"
#include "ae/physics/shape_primitive/ConePhysicsShape.h"
#include "ae/physics/shape_primitive/CylinderPhysicsShape.h"
#include "ae/physics/shape_primitive/PlanePhysicsShape.h"
#include "ae/physics/shape_primitive/SpherePhysicsShape.h"
#include "ae/rendering/Light.h"
#include "ae/rendering/Renderer.h"
#include "ae/rendering/VisualWorld.h"
#include "ae/rendering/camera/Camera.h"
#include "ae/rendering/camera/OrthographicCamera.h"
#include "ae/rendering/camera/PerspectiveCamera.h"
#include "ae/rendering/context/RenderContext.h"
#include "ae/rendering/context/platform/desktop/Window.h"
#include "ae/rendering/material/Material.h"
#include "ae/rendering/material/Sampleable.h"
#include "ae/rendering/material/Sampler.h"
#include "ae/rendering/material/Texture.h"
#include "ae/rendering/opengl/Program.h"
#include "ae/scene/Node.h"
#include "ae/scene/Scene.h"


#endif /* ae_h */
