//
//  Types.h
//  avara3d
//
//  Created by Morgan Davis on 10/8/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TYPES_H
#define AVARA3D_TYPES_H


#include <memory>
#include <set>
#include <unordered_set> // temporary?
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>

#include <glm/glm.hpp>


namespace a3d {


	class Color;
	class Texture;


/**************************************************************************************
	Public Type Utilities
 **************************************************************************************/

// works great in Linux, macOS, but typeof() is a GNU lanuage extension (C23)
//
//	#define A3D_MASK_CONTAINS(mask, bit) \
//		(static_cast<underlying_type<typeof(mask)>::type>(mask) \
//		& static_cast<underlying_type<typeof(mask)>::type>(bit))
//	#define A3D_MASK_ADD(mask, bit) \
//		(static_cast<typeof(mask)>(static_cast<underlying_type<typeof(mask)>::type>(mask) \
//		| static_cast<underlying_type<typeof(mask)>::type>(bit)))
//	#define A3D_MASK_REMOVE(mask, bit) \
//		(static_cast<typeof(mask)>(static_cast<underlying_type<typeof(mask)>::type>(mask) \
//		& ~ static_cast<underlying_type<typeof(mask)>::type>(bit)))

	#define A3D_MASK_CONTAINS(mask, bits) (static_cast<unsigned>(mask & bits) != 0)
	#define A3D_MASK_ADD(mask, bits) (mask | bits)
	#define A3D_MASK_REMOVE(mask, bits) (mask & ~bits)

	// example from fastgltf
	// a similar approach: https://stackoverflow.com/a/12080553

	template<typename T>
	constexpr std::underlying_type_t<T> to_underlying(T t) noexcept {
		return static_cast<std::underlying_type_t<T>>(t);
	}

	#define A3D_ENABLE_ARITHMETIC_OP(T1, T2, op) \
		constexpr T1 operator op(const T1& a, const T2& b) noexcept { \
			static_assert(std::is_enum_v<T1> && std::is_enum_v<T2>); \
			return static_cast<T1>(to_underlying(a) op to_underlying(b)); \
		}

	#define A3D_ENABLE_ASSIGNMENT_OP(T1, T2, op) \
		constexpr T1& operator op##=(T1& a, const T2& b) noexcept { \
			static_assert(std::is_enum_v<T1> && std::is_enum_v<T2>); \
			return a = static_cast<T1>(to_underlying(a) op to_underlying(b)), a; \
		}

	#define A3D_ENABLE_UNARY_OP(T, op) \
		constexpr T operator op(const T& a) noexcept { \
			static_assert(std::is_enum_v<T>); \
			return static_cast<T>(op to_underlying(a)); \
		}

	#define A3D_ENABLE_ENUM_MASK_OPS(T) \
		A3D_ENABLE_ARITHMETIC_OP(T, T, |) \
		A3D_ENABLE_ARITHMETIC_OP(T, T, &) \
		A3D_ENABLE_ASSIGNMENT_OP(T, T, |) \
		A3D_ENABLE_ASSIGNMENT_OP(T, T, &) \
		A3D_ENABLE_UNARY_OP(T, ~)

/**************************************************************************************
	Public Types
 **************************************************************************************/

	enum class LogLevel : unsigned {
		Trace =		0,
		Debug =		1,
		Info =		2,
		Warn =		3,
		Error =		4,
		Fatal = 	5,
		Off = 		6
	};

	enum class RenderingApi {
		OpenGL,
		OpenGLES,
		Vulkan
	};

	enum class SceneImportOptions : unsigned {
		None = 					0,
		ImportMeshes =			1 << 0,
		ImportMaterials =		1 << 1,
		ImportLights =			1 << 2,
		ImportCameras = 		1 << 3,
		ImportAll =				UINT_MAX
	};
	A3D_ENABLE_ENUM_MASK_OPS(SceneImportOptions)

	enum class MeshImportOptions : unsigned {
		None = 					0,
		ImportMaterials =		1 << 1, // note maps to SceneImportOptions
		ImportAll =				UINT_MAX
	};
	A3D_ENABLE_ENUM_MASK_OPS(MeshImportOptions)

	enum class FontType : unsigned {
		Unknown,
		OTF,
		TTF,
	};

	using MaterialProperty = std::variant<
			std::monostate,
			std::shared_ptr<Texture>,
			std::shared_ptr<Color>>;

	enum class MaterialPropertyType {
		Ambient,
		Diffuse,
		Specular,
		Emission
	};

	using MaterialPropertyList = std::vector<std::pair<const MaterialProperty*, MaterialPropertyType>>;

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

//	enum class LightType {
//		Ambient,
//		Point,
//		Directional,
//		Spot
//	};

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
		Primitive, // eh, do something else
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

	enum class WindowInputManagerErrorMask : unsigned {
		None =					0,
		NoMice =				1 << 0,
		PermissionDenied =		1 << 1,
		UnknownError =			1 << 2
	};
	A3D_ENABLE_ENUM_MASK_OPS(WindowInputManagerErrorMask)

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
		unsigned 	meshes;
		unsigned 	elements;
		unsigned 	polygons;
		unsigned 	lights;
		glm::vec3 	cameraPosition;

		unsigned	staticBodies;
		unsigned	dynamicBodies;
		unsigned	kinematicBodies;
		unsigned	primitiveShapes;
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
	A3D_ENABLE_ENUM_MASK_OPS(DebugOptions)

/**************************************************************************************
	Internal Types
 **************************************************************************************/

	enum class LightType {
		Ambient,
		Directional,
		Point,
		Spot
	};

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
	A3D_ENABLE_ENUM_MASK_OPS(NodeDirtyMask)

	enum class MeshDirtyMask : unsigned {
		None =					0,
		AABBLines	=			1 << 0,
		All = 					UINT_MAX
	};
	A3D_ENABLE_ENUM_MASK_OPS(MeshDirtyMask)

	enum class MeshElementDirtyMask : unsigned {
		None =					0,
		VertexData =			1 << 0,
		AABBLines	=			1 << 1,
		All = 					UINT_MAX
	};
	A3D_ENABLE_ENUM_MASK_OPS(MeshElementDirtyMask)

	enum class MaterialDirtyMask : unsigned {
		None =					0,
//		MaxAnisotropy = 		1 << 1,
		All = 					UINT_MAX
	};
	A3D_ENABLE_ENUM_MASK_OPS(MaterialDirtyMask)

	enum class TextureDirtyMask : unsigned {
		None =					0,
		Contents = 				1 << 0,
		All = 					UINT_MAX
	};
	A3D_ENABLE_ENUM_MASK_OPS(TextureDirtyMask)

	enum class SamplerDirtyMask : unsigned {
		None =					0,
		MinificationFilter = 	1 << 0,
		MagnificationFilter = 	1 << 1,
		MaxAnisotropy = 		1 << 2,
		WrapS = 				1 << 3,
		WrapT = 				1 << 4,
		WrapR = 				1 << 5,
		All = 					UINT_MAX
	};
	A3D_ENABLE_ENUM_MASK_OPS(SamplerDirtyMask)
}


#endif /* AVARA3D_TYPES_H */
