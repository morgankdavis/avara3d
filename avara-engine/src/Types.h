//
//  Types.h
//	avara-engine
//
//  Created by Morgan Davis on 10/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Types_h
#define Types_h

//#include <memory>

#include <glm/glm.hpp>


namespace ae {
	
	
	//class Node;
	
	
//	using NodeRef = std::shared_ptr<Node>;
//	using NodeWeakRef = std::weak_ptr<Node>;
//	//using NodeUniqueRef = std::unique_ptr<Node>;
	

//	typedef enum {
//		AntialiasingMode_None =	0,
//		AntialiasingMode_2X =	2,
//		AntialiasingMode_4X =	4,
//		AntialiasingMode_8X =	8,
//		AntialiasingMode_16X =	16
//	} AntialiasingMode;
	
	enum class ANTIALIASING_MODE : unsigned {
		NONE =		0,
		MSAA_2X =	2,
		MSAA_4X =	4,
		MSAA_8X =	8,
		MSAA_16X =	16
	};

	
//	typedef enum {
//		ShaderType_Vertex,
//		ShaderType_Fragment
//	} ShaderType;
	
	enum class SHADER_TYPE {
		VERTEX,
		FRAGMENT
	};
	
	
//	typedef enum {
//		MaterialPropertyType_Ambient,
//		MaterialPropertyType_Diffuse,
//		MaterialPropertyType_Specular,
//		MaterialPropertyType_Emissive
//	} MaterialPropertyType;
	
	enum class MATERIAL_PROPERTY_TYPE {
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		EMISSIVE
	};
	
	
//	typedef enum {
//		FillMode_Fill,
//		FillMode_Lines,
//		FillMode_Points
//	} FillMode;
	
	enum class FILL_MODE {
		FILL,
		LINES,
		POINTS
	};

	
//	typedef enum {
//		FilterMode_Nearest,
//		FilterMode_Linear,
//		FilterMode_NearestMipmapNearest,
//		FilterMode_LinearMipmapNearest,
//		FilterMode_NearestMipmapLinear,
//		FilterMode_LinearMipmapLinear
//	} FilterMode;
	
	enum class FILTER_MODE {
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR
	};
	
	
//	typedef enum {
//		WrapMode_ClampToEdge,
//		WrapMode_ClampToBorder,
//		WrapMode_Repeat,
//		WrapMode_MirroredRepeat
//	} WrapMode;
	
	enum class WRAP_MODE {
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		REPEAT,
		MIRRORED_REPEAT
	};


//	typedef enum {
//		LightType_Ambient,
//		LightType_Point,
//		LightType_Directional,
//		LightType_Spot
//	} LightType;
	
	enum class LIGHT_TYPE {
		AMBIENT,
		POINT,
		DIRECTIONAL,
		SPOT
	};


//	typedef enum {
//		DebugOption_ShowStatsOveray = 				1 << 0,
//		DebugOption_ShowBoundingBoxes = 			1 << 1,
//		DebugOption_ShowWireframes = 				1 << 2,
//		DebugOption_ShowCameras = 					1 << 3,
//		DebugOption_ShowLights = 					1 << 4,
//		DebugOption_ShowLightExtents = 				1 << 5,
//		DebugOption_ShowPhysicsBoundingBoxes = 		1 << 6,
//		DebugOption_ShowPhysicsWireframes = 		1 << 7,
//		DebugOption_ShowPhysicsContactPoints = 		1 << 8,
//		DebugOption_ShowPhysicsNormals = 			1 << 9,
//		DebugOption_ShowPhysicsConstraints =		1 << 10,
//		DebugOption_ShowPhysicsConstraintLimits	=	1 >> 11
//	} DebugOption;
	
	enum DEBUG_OPTIONS : unsigned {
		NONE =								1 << 0,
		SHOW_STATS_OVERLAY = 				1 << 1,
		SHOW_BOUNDING_BOXES = 				1 << 2,
		SHOW_WIREFRAMES = 					1 << 3,
		SHOW_CAMERAS = 						1 << 4,
		SHOW_LIGHTS = 						1 << 5,
		SHOW_LIGHT_EXTENTS = 				1 << 6,
		SHOW_PHYSICS_BOUNDING_BOXES = 		1 << 7,
		SHOW_PHYSICS_WIREFRAMES = 			1 << 8,
		SHOW_PHYSICS_CONTACT_POINTS = 		1 << 9,
		SHOW_PHYSICS_NORMALS = 				1 << 10,
		SHOW_PHYSICS_CONSTRAINTS =			1 << 11,
		SHOW_PHYSICS_CONSTRAINT_LIMITS	=	1 >> 12
	};
	
//#define DEBUG_OPTIONS_TO_RAW(option) (static_cast<unsigned>(option))
//#define DEBUG_OPTIONS_FROM_RAW(raw) (static_cast<DEBUG_OPTIONS>(raw))
#define DEBUG_OPTIONS_CONTAIN(options, option) (static_cast<unsigned>(options) & static_cast<unsigned>(option))
#define DEBUG_OPTIONS_ADD(options, option) (static_cast<DEBUG_OPTIONS>(static_cast<unsigned>(options) | static_cast<unsigned>(option)))
#define DEBUG_OPTIONS_REMOVE(options, option) (static_cast<DEBUG_OPTIONS>(static_cast<unsigned>(options) & ~ static_cast<unsigned>(option)))
	
//	inline bool DEBUG_OPTIONS_CONTAIN(DEBUG_OPTIONS options, DEBUG_OPTIONS option) { return (static_cast<int>(options) & static_cast<int>(option)); }
//	inline DEBUG_OPTIONS DEBUG_OPTIONS_ADD(DEBUG_OPTIONS options, DEBUG_OPTIONS option) { return static_cast<DEBUG_OPTIONS>(static_cast<int>(options) | static_cast<int>(option)); }
//	inline DEBUG_OPTIONS DEBUG_OPTIONS_REMOVE(DEBUG_OPTIONS options, DEBUG_OPTIONS option) { return static_cast<DEBUG_OPTIONS>(static_cast<int>(options) & ~ static_cast<int>(option)); }
	
	
//	typedef enum {
//		LoggerSink_MainFile =	1 << 0,
//		LoggerSink_NamedFile =	1 << 1,
//		LoggerSink_STDOUT = 	1 << 2
//	} LoggerSink;
	
	enum class LOGGER_SINKS : unsigned {
		NONE =			1 << 0,
		MAIN_FILE =		1 << 1,
		NAMED_FILE =	1 << 2,
		STDOUT = 		1 << 3
	};
	
#define LOGGER_SINKS_CONTAIN(sinks, sink) (static_cast<unsigned>(sinks) & static_cast<unsigned>(sink))
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
	
	
//	typedef enum {
//		PhysicsBodyType_Static =	0,
//		PhysicsBodyType_Dynamic =	1,
//		PhysicsBodyType_Kinematic =	2
//	} PhysicsBodyType;
	
	enum class PHYSICS_BODY_TYPE : unsigned {
		STATIC =	0,
		DYNAMIC =	1,
		KINEMATIC =	2
	};
	
	
//	typedef enum {
//		PhysicsShapeOption_Compound =	0,
//		PhysicsShapeOption_Type =		1
//	} PhysicsShapeOption;
	
	
//	typedef enum {
//		PhysicsShapeType_BoundingBox =			0,
//		PhysicsShapeType_ConcavePolyhedron =	1,
//		PhysicsShapeType_ConvexHull =			2
//	} PhysicsShapeType;
	
	enum class PHYSICS_SHAPE_TYPE : unsigned {
		BOUNDING_BOX =			0,
		CONVEX_HULL = 			1,
		CONCAVE_POLYHEDRON =	2
	};
	
#define PHYSICS_SHAPE_TYPE_TO_RAW(type) (static_cast<unsigned>(type))
#define PHYSICS_SHAPE_TYPE_FROM_RAW(raw) (static_cast<PHYSICS_SHAPE_TYPE>(raw))
	
	
//	typedef enum {
//		Key_Space = 32,
//		Key_Apostrophe = 39,  /* ' */
//		Key_Comma = 44,  /* , */
//		Key_Minus = 45,  /* - */
//		Key_period = 46,  /* . */
//		Key_Slash = 47,  /* / */
//		Key_0 = 48,
//		Key_1 = 49,
//		Key_2 = 50,
//		Key_3 = 51,
//		Key_4 = 52,
//		Key_5 = 53,
//		Key_6 = 54,
//		Key_7 = 55,
//		Key_8 = 56,
//		Key_9 = 57,
//		Key_Semicolon = 59,  /* ; */
//		Key_Equal = 61,  /* = */
//		Key_A = 65,
//		Key_B = 66,
//		Key_C = 67,
//		Key_D = 68,
//		Key_E = 69,
//		Key_F = 70,
//		Key_G = 71,
//		Key_H = 72,
//		Key_I = 73,
//		Key_J = 74,
//		Key_K = 75,
//		Key_L = 76,
//		Key_M = 77,
//		Key_N = 78,
//		Key_O = 79,
//		Key_P = 80,
//		Key_Q = 81,
//		Key_R = 82,
//		Key_S = 83,
//		Key_T = 84,
//		Key_U = 85,
//		Key_V = 86,
//		Key_W = 87,
//		Key_X = 88,
//		Key_Y = 89,
//		Key_Z = 90,
//		Key_LeftBracket = 91,  /* [ */
//		Key_Backslash = 92,  /* \ */
//		Key_RightBracket = 93,  /* ] */
//		Key_GraveAccent = 96,  /* ` */
//		Key_World1 = 161, /* non-US #1 */
//		Key_World2 = 162, /* non-US #2 */
//		
//		/* Function keys */
//		Key_Escape = 256,
//		Key_Enter = 257,
//		Key_Tab = 258,
//		Key_Backspace = 259,
//		Key_Insert = 260,
//		Key_Delete = 261,
//		Key_Right = 262,
//		Key_Left = 263,
//		Key_Down = 264,
//		Key_Up = 265,
//		Key_Page_Up = 266,
//		Key_Page_Down = 267,
//		Key_Home = 268,
//		Key_End = 269,
//		Key_Caps_Lock = 280,
//		Key_Scroll_Lock = 281,
//		Key_Num_Lock = 282,
//		Key_Print_Screen = 283,
//		Key_Pause = 284,
//		Key_F1 = 290,
//		Key_F2 = 291,
//		Key_F3 = 292,
//		Key_F4 = 293,
//		Key_F5 = 294,
//		Key_F6 = 295,
//		Key_F7 = 296,
//		Key_F8 = 297,
//		Key_F9 = 298,
//		Key_F10 = 299,
//		Key_F11 = 300,
//		Key_F12 = 301,
//		Key_F13 = 302,
//		Key_F14 = 303,
//		Key_F15 = 304,
//		Key_F16 = 305,
//		Key_F17 = 306,
//		Key_F18 = 307,
//		Key_F19 = 308,
//		Key_F20 = 309,
//		Key_F21 = 310,
//		Key_F22 = 311,
//		Key_F23 = 312,
//		Key_F24 = 313,
//		Key_F25 = 314,
//		Key_Keypad0 = 320,
//		Key_Keypad1 = 321,
//		Key_Keypad2 = 322,
//		Key_Keypad3 = 323,
//		Key_Keypad4 = 324,
//		Key_Keypad5 = 325,
//		Key_Keypad6 = 326,
//		Key_Keypad7 = 327,
//		Key_Keypad8 = 328,
//		Key_Keypad9 = 329,
//		Key_KeypadDecimal = 330,
//		Key_KeypadDivide = 331,
//		Key_KeypadMultiply = 332,
//		Key_KeypadSubtract = 333,
//		Key_KeypadAdd = 334,
//		Key_KeypadEnter = 335,
//		Key_KeypadEqual = 336,
//		Key_LeftShift = 340,
//		Key_LeftControl = 341,
//		Key_LeftAlt = 342,
//		Key_LeftSuper = 343,
//		Key_RightShift = 344,
//		Key_RightControl = 345,
//		Key_RightAlt = 346,
//		Key_RightSuper = 347,
//		Key_Menu = 348
//	} Key;
		
		
		
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
			DELETE = 261,
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
	
	
//	typedef enum {
//		ModifierKey_Shift = 0x0001,
//		ModifierKey_Control = 0x0002,
//		ModifierKey_Alt = 0x0004,
//		ModifierKey_Super = 0x0008
//	} ModifierKey;
	
	
//	typedef enum {
//		MouseButton_1 = 0,
//		MouseButton_2 = 1,
//		MouseButton_3 = 2,
//		MouseButton_4 = 3,
//		MouseButton_5 = 4,
//		MouseButton_6 = 5,
//		MouseButton_7 = 6,
//		MouseButton_8 = 7
//	} MouseButton;
	
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
	} DrawStats;
}


#endif /* Types_h */
