
//
//  Types.h
//	avara-engine
//
//  Created by Morgan Davis on 10/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Types_h
#define Types_h


#include <memory>
#include <set>
#include <typeinfo>

#include <glm/glm.hpp>


namespace ae {


/**************************************************************************************
	Public Type Utilities
 **************************************************************************************/

// works great in Linux, macOS, but typeof() is a GNU lanuage extension (C23)
//
//	#define AE_MASK_CONTAINS(mask, bit) \
//		(static_cast<underlying_type<typeof(mask)>::type>(mask) \
//		& static_cast<underlying_type<typeof(mask)>::type>(bit))
//	#define AE_MASK_ADD(mask, bit) \
//		(static_cast<typeof(mask)>(static_cast<underlying_type<typeof(mask)>::type>(mask) \
//		| static_cast<underlying_type<typeof(mask)>::type>(bit)))
//	#define AE_MASK_REMOVE(mask, bit) \
//		(static_cast<typeof(mask)>(static_cast<underlying_type<typeof(mask)>::type>(mask) \
//		& ~ static_cast<underlying_type<typeof(mask)>::type>(bit)))

	#define AE_MASK_CONTAINS(mask, bit) (static_cast<unsigned>(mask & bit) != 0)
	#define AE_MASK_ADD(mask, bit) 		(mask | bit)
	#define AE_MASK_REMOVE(mask, bit)	(mask & ~bit)

	// example from fastgltf
	// a similar approach: https://stackoverflow.com/a/12080553

	template<typename T>
	constexpr std::underlying_type_t<T> to_underlying(T t) noexcept {
		return static_cast<std::underlying_type_t<T>>(t);
	}

	#define AE_ENABLE_ARITHMETIC_OP(T1, T2, op) \
		constexpr T1 operator op(const T1& a, const T2& b) noexcept { \
			static_assert(std::is_enum_v<T1> && std::is_enum_v<T2>); \
			return static_cast<T1>(to_underlying(a) op to_underlying(b)); \
		}

	#define AE_ENABLE_ASSIGNMENT_OP(T1, T2, op) \
		constexpr T1& operator op##=(T1& a, const T2& b) noexcept { \
			static_assert(std::is_enum_v<T1> && std::is_enum_v<T2>); \
			return a = static_cast<T1>(to_underlying(a) op to_underlying(b)), a; \
		}

	#define AE_ENABLE_UNARY_OP(T, op) \
		constexpr T operator op(const T& a) noexcept { \
			static_assert(std::is_enum_v<T>); \
			return static_cast<T>(op to_underlying(a)); \
		}

/**************************************************************************************
	Public Types
 **************************************************************************************/

	enum class LogLevel : unsigned {
		Trace =		0,
		Debug =		1,
		Info =		2,
		Warn =		3,
		Error =		4,
		Critical = 	5,
		Off = 		6
	};

	enum class RenderingApi {
		OpenGL,
		OpenGLES,
		Vulkan
	};

	enum class SceneImportOptions : unsigned {
		None = 					0,
		ImportGeometries =		1 << 0,
		ImportMaterials =		1 << 1,
		ImportLights =			1 << 2,
		ImportCameras = 		1 << 3,
		ImportAll =				UINT_MAX
	};

	AE_ENABLE_ARITHMETIC_OP(SceneImportOptions, SceneImportOptions, |)
	AE_ENABLE_ARITHMETIC_OP(SceneImportOptions, SceneImportOptions, &)
	AE_ENABLE_ASSIGNMENT_OP(SceneImportOptions, SceneImportOptions, |)
	AE_ENABLE_ASSIGNMENT_OP(SceneImportOptions, SceneImportOptions, &)
	AE_ENABLE_UNARY_OP(SceneImportOptions, ~)

	enum class GeometryImportOptions : unsigned {
		None = 					0,
		ImportMaterials =		1 << 1, // note maps to SceneImportOptions
		ImportAll =				UINT_MAX
	};

	AE_ENABLE_ARITHMETIC_OP(GeometryImportOptions, GeometryImportOptions, |)
	AE_ENABLE_ARITHMETIC_OP(GeometryImportOptions, GeometryImportOptions, &)
	AE_ENABLE_ASSIGNMENT_OP(GeometryImportOptions, GeometryImportOptions, |)
	AE_ENABLE_ASSIGNMENT_OP(GeometryImportOptions, GeometryImportOptions, &)
	AE_ENABLE_UNARY_OP(GeometryImportOptions, ~)

	enum class FontType : unsigned {
		Unknown,
		OTF,
		TTF,
	};

	enum class MaterialPropertyType {
		Ambient,
		Diffuse,
		Specular,
		Emission
	};

	enum class FilterMode : unsigned {
		Nearest = 				0x2600,
		Linear = 				0x2601,
		NearestMipmapNearest = 	0x2700,
		LinearMipmapNearest = 	0x2701,
		NearestMipmapLinear = 	0x2702,
		LinearMipmapLinear = 	0x2703
	};

	enum class WrapMode : unsigned {
		Repeat = 			0x2901,
		MirroredRepeat = 	0x8370,
		ClampToEdge = 		0x812F
	};

	enum class BlendFunction {
		Disabled
	};

	enum class LightType {
		Ambient,
		Point,
		Directional,
		Spot
	};

	enum class AntialiasingMode : unsigned {
		None =		0,
		Msaa2X =	2,
		Msaa4X =	4,
		Msaa8X =	8,
		Msaa16X =	16
	};

	enum class PhysicsBodyType : unsigned {
		Static,
		Dynamic,
		Kinematic
	};

	enum class PhysicsShapeType : unsigned {
		BoundingBox,
		ConvexHull,
		ConcavePolyhedron
	};

	enum class Key : int {
		Unknown = 0,
		Space = 32,
		Apostrophe = 39,
		Comma = 44,
		Minus = 45,
		Period = 46,
		Slash = 47,
		Zero = 48,
		One = 49,
		Two = 50,
		Three = 51,
		Four = 52,
		Five = 53,
		Six = 54,
		Seven = 55,
		Eight = 56,
		Nine = 57,
		Semicolon = 59,
		Equal = 61,
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		LeftBracket = 91,
		Backslash = 92,
		RightBracket = 93,
		GraveAccent = 96,
		World1 = 161, // non-US #1
		World2 = 162, // non-US #2
		
		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		ForwardDelete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,
		Keypad0 = 320,
		Keypad1 = 321,
		Keypad2 = 322,
		Keypad3 = 323,
		Keypad4 = 324,
		Keypad5 = 325,
		Keypad6 = 326,
		Keypad7 = 327,
		Keypad8 = 328,
		Keypad9 = 329,
		KeypadDecimal = 330,
		KeypadDivide = 331,
		KeypadMultiply = 332,
		KeypadSubtract = 333,
		KeypadAdd = 334,
		KeypadEnter = 335,
		KeypadEqual = 336,
		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	};
		
	enum class MouseButton : int {
		One = 	0,
		Two = 	1,
		Three = 2,
		Four = 	3,
		Five = 	4,
		Six = 	5,
		Seven = 6,
		Eight = 7
	};

	typedef struct {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	} Vertex;

	typedef struct {
		unsigned a;
		unsigned b;
		unsigned c;
	} Face;

	typedef struct {
		glm::vec3 min;
		glm::vec3 max;
	} AABB;

	typedef struct {
		float x;
		float y;
		float z;
	} Extent;

	typedef struct {
		// the frame time as of the last frame in ms
		double		currentFrametime;
		// the framerate as of the last frame in frames/second
		double		currentFramerate;
		// the average of frame time over averagingInterval in ms
		double		averageFrametime;
		// the average of frame rate over averagingInterval in frames/second
		double		averageFramerate;
		// time interval over which to average averageFrametime, averageFramerate,
		// averagePhysicstime, and averageDrawtime over in seconds
		double 		averagingInterval;

		double		currentPhysicstime;
		double		currentDrawtime;
		double		currentUsertime;
		double 		averagePhysicstime;
		double		averageDrawtime;
		double		averageUsertime;

		unsigned 	nodes;
		unsigned 	geometries;
		unsigned 	meshes;
		unsigned 	polygons;
		unsigned 	lights;
		glm::vec3 	cameraPosition;

		unsigned	staticBodies;
		unsigned	dynamicBodies;
		unsigned	kinematicBodies;
		unsigned	boundingBoxShapes;
		unsigned	convexHullShapes;
		unsigned	concavePolyhedronShapes;
	} Stats;

	enum DebugOptions : unsigned {
		None =							0,
		ShowStatsOverlay = 				1 << 0,
		ShowBoundingBoxes = 			1 << 1,
		ShowWireframes = 				1 << 2,
		ShowCameras = 					1 << 3,
		ShowLights = 					1 << 4,
		ShowLightExtents = 				1 << 5,
		ShowPhysicsBoundingBoxes = 		1 << 6,
		ShowPhysicsWireframes = 		1 << 7,
		ShowPhysicsContactPoints = 		1 << 8,
		ShowPhysicsNormals = 			1 << 9,
		ShowPhysicsConstraints =		1 << 10,
		ShowPhysicsConstraintLimits	=	1 << 11
	};

	AE_ENABLE_ARITHMETIC_OP(DebugOptions, DebugOptions, |)
	AE_ENABLE_ARITHMETIC_OP(DebugOptions, DebugOptions, &)
	AE_ENABLE_ASSIGNMENT_OP(DebugOptions, DebugOptions, |)
	AE_ENABLE_ASSIGNMENT_OP(DebugOptions, DebugOptions, &)
	AE_ENABLE_UNARY_OP(DebugOptions, ~)

/**************************************************************************************
	Internal
 **************************************************************************************/

	class Line;
	class Point;
	using LineSet = std::set<std::shared_ptr<Line>>;
	using PointSet = std::set<std::shared_ptr<Point>>;

	enum class ShaderType {
		Vertex,
		Fragment
	};

	enum class FillMode {
		Fill,
		Lines,
		Points
	};
		
	enum class NodeDirtyMask : unsigned {
		None =					0,
		WorldTransform =		1 << 0,
		All = 					UINT_MAX
	};

	AE_ENABLE_ARITHMETIC_OP(NodeDirtyMask, NodeDirtyMask, |)
	AE_ENABLE_ARITHMETIC_OP(NodeDirtyMask, NodeDirtyMask, &)
	AE_ENABLE_ASSIGNMENT_OP(NodeDirtyMask, NodeDirtyMask, |)
	AE_ENABLE_ASSIGNMENT_OP(NodeDirtyMask, NodeDirtyMask, &)
	AE_ENABLE_UNARY_OP(NodeDirtyMask, ~)

	enum class GeometryDirtyMask : unsigned {
		None =					0,
		Extent =				1 << 0,
		All = 					UINT_MAX
	};

	AE_ENABLE_ARITHMETIC_OP(GeometryDirtyMask, GeometryDirtyMask, |)
	AE_ENABLE_ARITHMETIC_OP(GeometryDirtyMask, GeometryDirtyMask, &)
	AE_ENABLE_ASSIGNMENT_OP(GeometryDirtyMask, GeometryDirtyMask, |)
	AE_ENABLE_ASSIGNMENT_OP(GeometryDirtyMask, GeometryDirtyMask, &)
	AE_ENABLE_UNARY_OP(GeometryDirtyMask, ~)

	enum class GeometryElementDirtyMask : unsigned {
		None =					0,
		VertexData =			1 << 0,
		All = 					UINT_MAX
	};

	AE_ENABLE_ARITHMETIC_OP(GeometryElementDirtyMask, GeometryElementDirtyMask, |)
	AE_ENABLE_ARITHMETIC_OP(GeometryElementDirtyMask, GeometryElementDirtyMask, &)
	AE_ENABLE_ASSIGNMENT_OP(GeometryElementDirtyMask, GeometryElementDirtyMask, |)
	AE_ENABLE_ASSIGNMENT_OP(GeometryElementDirtyMask, GeometryElementDirtyMask, &)
	AE_ENABLE_UNARY_OP(GeometryElementDirtyMask, ~)

	enum class MaterialDirtyMask : unsigned {
		None =					0,
		All = 					UINT_MAX
	};

	AE_ENABLE_ARITHMETIC_OP(MaterialDirtyMask, MaterialDirtyMask, |)
	AE_ENABLE_ARITHMETIC_OP(MaterialDirtyMask, MaterialDirtyMask, &)
	AE_ENABLE_ASSIGNMENT_OP(MaterialDirtyMask, MaterialDirtyMask, |)
	AE_ENABLE_ASSIGNMENT_OP(MaterialDirtyMask, MaterialDirtyMask, &)
	AE_ENABLE_UNARY_OP(MaterialDirtyMask, ~)

//	enum class MaterialPropertyDirtyMask : unsigned {
//		None =					0,
//		Contents = 				1 << 0,
//		MinificationFilter = 	1 << 1,
//		MagnificationFilter = 	1 << 2,
//		WrapS = 				1 << 3,
//		WrapT = 				1 << 4,
//		WrapR = 				1 << 5,
//		MaxAnisotropy = 		1 << 6,
//		All = 					UINT_MAX
//	};
//
//	AE_ENABLE_ARITHMETIC_OP(MaterialPropertyDirtyMask, MaterialPropertyDirtyMask, |)
//	AE_ENABLE_ARITHMETIC_OP(MaterialPropertyDirtyMask, MaterialPropertyDirtyMask, &)
//	AE_ENABLE_ASSIGNMENT_OP(MaterialPropertyDirtyMask, MaterialPropertyDirtyMask, |)
//	AE_ENABLE_ASSIGNMENT_OP(MaterialPropertyDirtyMask, MaterialPropertyDirtyMask, &)
//	AE_ENABLE_UNARY_OP(MaterialPropertyDirtyMask, ~)

	enum class SamplerDirtyMask : unsigned {
		None =					0,
//		Contents = 				1 << 0,
		MinificationFilter = 	1 << 1,
		MagnificationFilter = 	1 << 2,
		WrapS = 				1 << 3,
		WrapT = 				1 << 4,
		WrapR = 				1 << 5,
		MaxAnisotropy = 		1 << 6,
		All = 					UINT_MAX
	};

	AE_ENABLE_ARITHMETIC_OP(SamplerDirtyMask, SamplerDirtyMask, |)
	AE_ENABLE_ARITHMETIC_OP(SamplerDirtyMask, SamplerDirtyMask, &)
	AE_ENABLE_ASSIGNMENT_OP(SamplerDirtyMask, SamplerDirtyMask, |)
	AE_ENABLE_ASSIGNMENT_OP(SamplerDirtyMask, SamplerDirtyMask, &)
	AE_ENABLE_UNARY_OP(SamplerDirtyMask, ~)

} // namespace ae


#endif /* Types_h */
