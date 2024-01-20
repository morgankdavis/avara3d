//
//  ae.h
//  avara-engine
//
//  Created by Morgan Davis on 9/20/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef ae_h
#define ae_h


namespace ae {

	struct Version { // semver.org
		int major;
		int minor;
		int patch;
		int build; // move to BuildInfo?
	};

	struct BuildInfo {

		enum class TYPE {
			DEBUG,
			RELEASE
		};

		enum class ORIGIN {
			AUTOMATED,
			ADHOC
		};

		static BuildInfo& Info();

		Version version;
		TYPE type;
		ORIGIN origin;
//		std::time date; // better type?
	};

	extern constexpr int MajorVersion();
	extern constexpr int MinorVersion();
	extern constexpr int PatchVersion();
	extern constexpr int BuildNumber();
	extern constexpr const unsigned char* VersionString(); // 1.2.3
	extern constexpr const unsigned char* BuildString(); // 12345
}

#include "Global.h"
#include "Types.h"
#include "diagnostic/Exception.h"
#include "diagnostic/logging/Logger.h"
#include "diagnostic/logging/sinks/FileLoggerSink.h"
#if defined(ANDROID)
#include "diagnostic/logging/sinks/platform/android/FileLoggerSink.h"
#elif defined(DESKTOP)
#include "diagnostic/logging/sinks/platform/desktop/StdOutLoggerSink.h"
#endif
#include "geometry/Geometry.h"
#include "geometry/GeometryElement.h"
#include "geometry/Line.h"
#include "geometry/Point.h"
#include "geometry/primitives/Box.h"
#include "geometry/primitives/Capsule.h"
#include "geometry/primitives/Cone.h"
#include "geometry/primitives/Cylinder.h"
#include "geometry/primitives/Plane.h"
#include "geometry/primitives/Pyramid.h"
#include "geometry/primitives/Sphere.h"
#include "geometry/primitives/Torus.h"
#include "geometry/primitives/Tube.h"
#include "input/InputManager.h"
#include "input/platform/desktop/WindowInputManager.h" // remove?
#include "physics/HitTestResult.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsContact.h"
#include "physics/PhysicsShape.h"
#include "physics/PhysicalWorld.h"
#include "physics/shape_primitives/BoxPhysicsShape.h"
#include "physics/shape_primitives/CapsulePhysicsShape.h"
#include "physics/shape_primitives/ConePhysicsShape.h"
#include "physics/shape_primitives/CylinderPhysicsShape.h"
#include "physics/shape_primitives/PlanePhysicsShape.h"
#include "physics/shape_primitives/SpherePhysicsShape.h"
#include "rendering/Light.h"
#include "rendering/Renderer.h"
#include "rendering/VisualWorld.h"
#include "rendering/camera/Camera.h"
#include "rendering/camera/OrthographicCamera.h"
#include "rendering/camera/PerspectiveCamera.h"
#include "rendering/context/RenderContext.h"
#include "rendering/context/platform/desktop/Window.h"
#include "rendering/materials/Material.h"
#include "rendering/materials/MaterialProperty.h"
#include "rendering/materials/MaterialPropertyContents.h"
#include "rendering/opengl/Program.h"
#include "scene/Node.h"
#include "scene/Scene.h"
#include "utilities/Buffer.h"
#include "utilities/Color.h"
#include "utilities/CubeImage.h"
#include "utilities/Font.h"
#include "utilities/Image.h"


#endif /* ae_h */
