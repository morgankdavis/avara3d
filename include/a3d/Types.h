//
//  Types.h
//  avara3d
//
//  Created by Morgan Davis on 10/8/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TYPES_H
#define AVARA3D_TYPES_H

#include <cstdint>
#include <climits>
#include <cstdint>
#include <memory>
#include <set>
#include <unordered_set>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>

#include "a3d/Math.h"
#include "a3d/util/bitmask.h"

namespace a3d {

	class Color;
	class CubeImage;
	class Image;
	class Texture;

	/// Public Type Utilities ///



	/// Public Types ///

	enum class LogLevel : uint8_t {
		Trace =		0,
		Debug =		1,
		Info =		2,
		Warn =		3,
		Error =		4,
		Fatal = 	5,
		Off = 		6
	};

	enum class RenderingApi : uint8_t {
		OpenGL,
		OpenGLES,
		Vulkan
	};

	enum class SceneImportOptions : uint16_t {
		None = 					0,
		ImportMeshes =			1 << 0,
		ImportMaterials =		1 << 1,
		ImportLights =			1 << 2,
		ImportCameras = 		1 << 3,
		ImportAll =				UINT16_MAX
	};
	namespace util::bitmask {
		template <> struct enable_ops<SceneImportOptions> : std::true_type {};
	}

	enum class MeshImportOptions : uint16_t {
		None = 					0,
		ImportMaterials =		1 << 1, // note maps to SceneImportOptions
		ImportAll =				UINT16_MAX
	};
	namespace util::bitmask {
		template <> struct enable_ops<MeshImportOptions> : std::true_type {};
	}

	enum class FontType : uint8_t {
		Unknown,
		OTF,
		TTF,
	};

	using MaterialProperty = std::variant<
			std::monostate,
			std::shared_ptr<Texture>,
			std::shared_ptr<Color>>;

	using Sampleable = std::variant<
			std::monostate,
			std::shared_ptr<Image>,
			std::shared_ptr<CubeImage>>;

	enum class MaterialPropertyType : uint8_t {
		Ambient = 	0,
		Diffuse = 	1,
		Specular =	2,
		Emission =	3
	};

	using MaterialPropertyList = std::vector<std::pair<const MaterialProperty*, MaterialPropertyType>>;

	enum class FilterMode : uint16_t {
		Nearest = 				0x2600,
		Linear = 				0x2601,
		NearestMipmapNearest = 	0x2700,
		LinearMipmapNearest = 	0x2701,
		NearestMipmapLinear = 	0x2702,
		LinearMipmapLinear = 	0x2703
	};

	enum class WrapMode : uint16_t {
		Repeat = 			0x2901,
		MirroredRepeat = 	0x8370,
		ClampToEdge = 		0x812F
	};

	enum class AlphaMode : uint8_t {
		Opaque, // no discard, no blending
		Mask, // uses discard/alpha threshold
		Blend }; // real transparency (glBlend enabled + depthWrite off)

	enum class BlendFunction : uint8_t {
		Disabled,
		Alpha,
		Additive,
		PremultipliedAlpha
	};

	enum class SpotlightFeatheringMode : uint8_t {
		Linear =	0,
		Sharp = 	1, // y = x(2-x)
		Soft = 		2 // y = x^2
	};

	struct AttenuationLightCutoff {
		// cuts off light when attenuation drops below 'attenuation'.
		// computed per-fragment.
		float attenuation;
	};

	struct DistanceLightCutoff {
		// cuts lights off past 'distance'.
		// computed per-fragment.
		float distance;
	};

	using LightCutoff = std::variant<
			std::monostate,
			AttenuationLightCutoff,
			DistanceLightCutoff>;

	enum class AntialiasingMode : uint8_t {
		None =		0,
		Msaa2X =	2,
		Msaa4X =	4,
		Msaa8X =	8,
		Msaa16X =	16
	};

	enum class PhysicsBodyType : uint8_t {
		Static,
		Dynamic,
		Kinematic
	};

	enum class PhysicsShapeType : uint8_t {
		Primitive, // eh, do something else
		BoundingBox,
		ConvexHull,
		ConcavePolyhedron
	};

	enum class Key : int {
		Unknown = 0,
		Space = 32, //+
		Apostrophe = 39, //+
		Comma = 44, //+
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

	struct Face {
		uint32_t a;
		uint32_t b;
		uint32_t c;
	};

	enum DebugOptions : uint32_t {
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
	namespace util::bitmask {
		template <> struct enable_ops<DebugOptions> : std::true_type {};
	}

	/// Internal Types ///

	using MeshId = uint32_t;
	using MaterialId = uint32_t;
	using TextureId = uint32_t;
	using SamplerId = uint32_t;

	enum class ShaderType {
		Vertex,
		Fragment
	};

	enum class FillMode {
		Fill,
		Lines,
		Points
	};

	enum class RenderStyle : uint8_t {
		Normal,
		Wireframe,
		WireframeOverlay
	};
		
	enum class NodeDirtyMask : uint32_t {
		None =					0,
		WorldTransform =		1 << 0,
		All = 					UINT_MAX
	};
	namespace util::bitmask {
		template <> struct enable_ops<NodeDirtyMask> : std::true_type {};
	}

	enum class MeshDirtyMask : uint32_t {
		None =					0,
//		AABBLines	=			1 << 0,
		All = 					UINT_MAX
	};
	namespace util::bitmask {
		template <> struct enable_ops<MeshDirtyMask> : std::true_type {};
	}

	enum class MeshElementDirtyMask : uint32_t {
		None =					0,
		VertexData =			1 << 0,
//		AABBLines	=			1 << 1,
		All = 					UINT_MAX
	};
	namespace util::bitmask {
		template <> struct enable_ops<MeshElementDirtyMask> : std::true_type {};
	}

	enum class MaterialDirtyMask : uint32_t {
		None =					0,
//		MaxAnisotropy = 		1 << 1,
		All = 					UINT_MAX
	};
	namespace util::bitmask {
		template <> struct enable_ops<MaterialDirtyMask> : std::true_type {};
	}

	enum class TextureDirtyMask : uint32_t {
		None =					0,
		Contents = 				1 << 0,
		Sampler =				1 << 1, // TODO: move to MaterialBinding dirty mask?
		All = 					UINT_MAX
	};
	namespace util::bitmask {
		template <> struct enable_ops<TextureDirtyMask> : std::true_type {};
	}

	enum class SamplerDirtyMask : uint32_t {
		None =					0,
		MinificationFilter = 	1 << 0,
		MagnificationFilter = 	1 << 1,
		MaxAnisotropy = 		1 << 2,
		WrapS = 				1 << 3,
		WrapT = 				1 << 4,
		WrapR = 				1 << 5,
		All = 					UINT_MAX
	};
	namespace util::bitmask {
		template <> struct enable_ops<SamplerDirtyMask> : std::true_type {};
	}

//	enum class VisualWorldDirtyMask : unsigned {
//		None =					0,
//		Background = 			1 << 0,
//		All = 					UINT_MAX
//	};
//	A3D_ENABLE_ENUM_MASK_OPS(VisualWorldDirtyMask)
}

#endif /* AVARA3D_TYPES_H */
