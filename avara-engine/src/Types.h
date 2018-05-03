//
//  Types.h
//	avara-engine
//
//  Created by Morgan Davis on 10/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Types_h
#define Types_h


#include <glm/glm.hpp>


namespace ae {
	
	typedef unsigned GEOMETRY_ID;
	typedef unsigned GEOMETRY_ELEMENT_ID;
	typedef unsigned MATERIAL_ID;
	typedef unsigned MATERIAL_PROPERTY_ID;

	enum class ANTIALIASING_MODE : unsigned {
		NONE =		0,
		MSAA_2X =	2,
		MSAA_4X =	4,
		MSAA_8X =	8,
		MSAA_16X =	16
	};

	enum class SHADER_TYPE {
		VERTEX,
		FRAGMENT
	};

	enum class MATERIAL_PROPERTY_TYPE {
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		EMISSIVE
	};

	enum class FILL_MODE {
		FILL,
		LINES,
		POINTS
	};

	enum class FILTER_MODE {
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR
	};

	enum class WRAP_MODE {
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		REPEAT,
		MIRRORED_REPEAT
	};

	enum class LIGHT_TYPE {
		AMBIENT,
		POINT,
		DIRECTIONAL,
		SPOT
	};

	enum DEBUG_OPTIONS : unsigned {
		NONE =								0,
		SHOW_STATS_OVERLAY = 				1 << 0,
		SHOW_BOUNDING_BOXES = 				1 << 1,
		SHOW_WIREFRAMES = 					1 << 2,
		SHOW_CAMERAS = 						1 << 3,
		SHOW_LIGHTS = 						1 << 4,
		SHOW_LIGHT_EXTENTS = 				1 << 5,
		SHOW_PHYSICS_BOUNDING_BOXES = 		1 << 6,
		SHOW_PHYSICS_WIREFRAMES = 			1 << 7,
		SHOW_PHYSICS_CONTACT_POINTS = 		1 << 8,
		SHOW_PHYSICS_NORMALS = 				1 << 9,
		SHOW_PHYSICS_CONSTRAINTS =			1 << 10,
		SHOW_PHYSICS_CONSTRAINT_LIMITS	=	1 >> 11
	};
	
#define DEBUG_OPTIONS_CONTAINS(options, option) (static_cast<unsigned>(options) & static_cast<unsigned>(option))
#define DEBUG_OPTIONS_ADD(options, option) (static_cast<DEBUG_OPTIONS>(static_cast<unsigned>(options) | static_cast<unsigned>(option)))
#define DEBUG_OPTIONS_REMOVE(options, option) (static_cast<DEBUG_OPTIONS>(static_cast<unsigned>(options) & ~ static_cast<unsigned>(option)))
	
	enum class LOGGER_SINKS : unsigned {
		NONE =			0,
		MAIN_FILE =		1 << 0,
		NAMED_FILE =	1 << 1,
		STDOUT = 		1 << 2
	};
	
#define LOGGER_SINKS_CONTAINS(sinks, sink) (static_cast<unsigned>(sinks) & static_cast<unsigned>(sink))
#define LOGGER_SINKS_ADD(sinks, sink) (static_cast<LOGGER_SINKS>(static_cast<unsigned>(sinks) | static_cast<unsigned>(sink)))
#define LOGGER_SINKS_REMOVE(sinks, sink) (static_cast<LOGGER_SINKS>(static_cast<unsigned>(sinks) & ~ static_cast<unsigned>(sink)))

	typedef struct {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoordinate;
	} Vertex;
	
	typedef struct {
		unsigned a;
		unsigned b;
		unsigned c;
	} Face;
	
	enum class PHYSICS_BODY_TYPE : unsigned {
		STATIC =	0,
		DYNAMIC =	1,
		KINEMATIC =	2
	};

	enum class PHYSICS_SHAPE_TYPE : unsigned {
		BOUNDING_BOX =			0,
		CONVEX_HULL = 			1,
		CONCAVE_POLYHEDRON =	2
	};
	
// get rid of this? or make this patter universal...
#define PHYSICS_SHAPE_TYPE_TO_RAW(type) (static_cast<unsigned>(type))
#define PHYSICS_SHAPE_TYPE_FROM_RAW(raw) (static_cast<PHYSICS_SHAPE_TYPE>(raw))
	
	enum class KEY : int {
		SPACE = 32,
		APOSTROPHE = 39,  /* ' */
		COMMA = 44,  /* , */
		MINUS = 45,  /* - */
		PERIOD = 46,  /* . */
		SLASH = 47,  /* / */
		ZERO = 48,
		ONE = 49,
		TWO = 50,
		THREE = 51,
		FOUR = 52,
		FIVE = 53,
		SIX = 54,
		SEVEN = 55,
		EIGHT = 56,
		NINE = 57,
		SEMICOLON = 59,  /* ; */
		EQUAL = 61,  /* = */
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
		LEFT_BRACKET = 91,  /* [ */
		BACKSLASH = 92,  /* \ */
		RIGHT_BRACKET = 93,  /* ] */
		GRACE_ACCENT = 96,  /* ` */
		WORLD_1 = 161, /* non-US #1 */
		WORLD_2 = 162, /* non-US #2 */
		
		/* Function keys */
		ESCAPE = 256,
		ENTER = 257,
		TAB = 258,
		BACKSPACE = 259,
		INSERT = 260,
		DEL = 261,
		RIGHT = 262,
		LEFT = 263,
		DOWN = 264,
		UP = 265,
		PAGE_UP = 266,
		PAGE_DOWN = 267,
		HOME = 268,
		END = 269,
		CAPS_LOCK = 280,
		SCROLL_LOCK = 281,
		NUM_LOCK = 282,
		PRINT_SCREEN = 283,
		PAUSE = 284,
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
		KEYPAD_0 = 320,
		KEYPAD_1 = 321,
		KEYPAD_2 = 322,
		KEYPAD_3 = 323,
		KEYPAD_4 = 324,
		KEYPAD_5 = 325,
		KEYPAD_6 = 326,
		KEYPAD_7 = 327,
		KEYPAD_8 = 328,
		KEYPAD_9 = 329,
		KEYPAD_DECIMAL = 330,
		KEYPAD_DIVIDE = 331,
		KEYPAD_MULTIPLY = 332,
		KEYPAD_SUBTRACT = 333,
		KEYPAD_ADD = 334,
		KEYPAD_ENTER = 335,
		KEYPAD_EQUAL = 336,
		LEFT_SHIFT = 340,
		KEFT_CONTROL = 341,
		LEFT_ALT = 342,
		LEFT_SUPER = 343,
		RIGHT_SHIFT = 344,
		RIGHT_CONTROL = 345,
		RIGHT_ALT = 346,
		RIGHT_SUPER = 347,
		MENU = 348
	};
		
//#define KEY_TO_RAW(key) (static_cast<int>(key))
//#define KEY_FROM_RAW(raw) (static_cast<KEY>(raw))

	enum class MOUSE_BUTTON : int {
		ONE = 	0,
		TWO = 	1,
		THREE = 2,
		FOUR = 	3,
		FIVE = 	4,
		SIX = 	5,
		SEVEN = 6,
		EIGHT = 7
		};
	
//#define MOUSE_BUTTON_TO_RAW(button) (static_cast<int>(button))
//#define MOUSE_BUTTON_FROM_RAW(raw) (static_cast<MOUSE_BUTTON>(raw))
	
	typedef struct {
		unsigned nodes;
		unsigned geometries;
		unsigned meshes;
		unsigned polygons;
		unsigned lights;
		glm::vec3 cameraPosition;
	} RenderStats;
		
	
	enum class GEOMETRY_DIRTY_BITS : unsigned {
		WORLD_TRANSFORM =		1 << 0, // not currently used
		AABB =					1 << 1,
		ALL = 					UINT_MAX
	};
		
#define GEOMETRY_DIRTY_BITS_CONTAINS(bits, bit) (static_cast<unsigned>(bits) & static_cast<unsigned>(bit))
#define GEOMETRY_DIRTY_BITS_ADD(bits, bit) (static_cast<GEOMETRY_DIRTY_BITS>(static_cast<unsigned>(bits) | static_cast<unsigned>(bit)))
#define GEOMETRY_DIRTY_BITS_REMOVE(bits, bit) (static_cast<GEOMETRY_DIRTY_BITS>(static_cast<unsigned>(bits) & ~ static_cast<unsigned>(bit)))

	enum class GEOMETRY_ELEMENT_DIRTY_BITS : unsigned {
		VERTEX_DATA =		1 << 0,
		ALL = 				UINT_MAX
	};
		
#define GEOMETRY_ELEMENT_DIRTY_BITS_CONTAINS(bits, bit) (static_cast<unsigned>(bits) & static_cast<unsigned>(bit))
#define GEOMETRY_ELEMENT_DIRTY_BITS_ADD(bits, bit) (static_cast<GEOMETRY_ELEMENT_DIRTY_BITS>(static_cast<unsigned>(bits) | static_cast<unsigned>(bit)))
#define GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(bits, bit) (static_cast<GEOMETRY_ELEMENT_DIRTY_BITS>(static_cast<unsigned>(bits) & ~ static_cast<unsigned>(bit)))
	
	enum class MATERIAL_DIRTY_BITS : unsigned {
		ALL = 				UINT_MAX
	};
		
#define MATERIAL_DIRTY_BITS_CONTAINS(bits, bit) (static_cast<unsigned>(bits) & static_cast<unsigned>(bit))
#define MATERIAL_DIRTY_BITS_ADD(bits, bit) (static_cast<MATERIAL_DIRTY_BITS>(static_cast<unsigned>(bits) | static_cast<unsigned>(bit)))
#define MATERIAL_DIRTY_BITS_REMOVE(bits, bit) (static_cast<MATERIAL_DIRTY_BITS>(static_cast<unsigned>(bits) & ~ static_cast<unsigned>(bit)))
	
	enum class MATERIAL_PROPERTY_DIRTY_BITS : unsigned {
		CONTENTS = 				1 << 0,
		MINIFICATION_FILTER = 	1 << 1,
		MAGNIFICATION_FILTER = 	1 << 2,
		WRAP_S = 				1 << 3,
		WRAP_T = 				1 << 4,
		WRAP_R = 				1 << 5,
		MAX_ANISTROPY = 		1 << 6,
		ALL = 					UINT_MAX
	};
		
#define MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(bits, bit) (static_cast<unsigned>(bits) & static_cast<unsigned>(bit))
#define MATERIAL_PROPERTY_DIRTY_BITS_ADD(bits, bit) (static_cast<MATERIAL_PROPERTY_DIRTY_BITS>(static_cast<unsigned>(bits) | static_cast<unsigned>(bit)))
#define MATERIAL_PROPERTY_DIRTY_BITS_REMOVE(bits, bit) (static_cast<MATERIAL_PROPERTY_DIRTY_BITS>(static_cast<unsigned>(bits) & ~ static_cast<unsigned>(bit)))
}


#endif /* Types_h */
