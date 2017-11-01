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

	typedef enum {
		AntialiasingMode_None =	0,
		AntialiasingMode_2X =	2,
		AntialiasingMode_4X =	4,
		AntialiasingMode_8X =	8,
		AntialiasingMode_16X =	16
	} AntialiasingMode;

//	typedef enum {
//		SceneLoadingOption_ValidateStructure,
//		SceneLoadingOption_PreTransform,
//		SceneLoadingOption_ImproveCacheLocality
//	} SceneLoadingOption;

	typedef enum {
		ProgramType_Default,
		ProgramType_PhongPlain, // temporary
		ProgramType_PhongTexture, // temporary
		ProgramType_Fancy // temporary
	} ProgramType;

	typedef enum {
		WrapMode_Clamp,
		WrapMode_Repeat,
		WrapMode_lampToBorder,
		WrapMode_Mirror
	} WrapMode;


	typedef enum {
		WrapMode_None,
		WrapMode_Nearest,
		WrapMode_Linear
	} FilterMode;


	typedef enum {
		LightType_Ambient,
		LightType_Omni,
		LightType_Directional,
		LightType_Spot
	} LightType;


	typedef struct {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoordinate;
	} Vertex;

	typedef struct {
		unsigned int a;
		unsigned int b;
		unsigned int c;
	} Face;
	
	
	typedef enum {
		Key_Space = 32,
		Key_Apostrophe = 39,  /* ' */
		Key_Comma = 44,  /* , */
		Key_Minus = 45,  /* - */
		Key_period = 46,  /* . */
		Key_Slash = 47,  /* / */
		Key_0 = 48,
		Key_1 = 49,
		Key_2 = 50,
		Key_3 = 51,
		Key_4 = 52,
		Key_5 = 53,
		Key_6 = 54,
		Key_7 = 55,
		Key_8 = 56,
		Key_9 = 57,
		Key_Semicolon = 59,  /* ; */
		Key_Equal = 61,  /* = */
		Key_A = 65,
		Key_B = 66,
		Key_C = 67,
		Key_D = 68,
		Key_E = 69,
		Key_F = 70,
		Key_G = 71,
		Key_H = 72,
		Key_I = 73,
		Key_J = 74,
		Key_K = 75,
		Key_L = 76,
		Key_M = 77,
		Key_N = 78,
		Key_O = 79,
		Key_P = 80,
		Key_Q = 81,
		Key_R = 82,
		Key_S = 83,
		Key_T = 84,
		Key_U = 85,
		Key_V = 86,
		Key_W = 87,
		Key_X = 88,
		Key_Y = 89,
		Key_Z = 90,
		Key_LeftBracket = 91,  /* [ */
		Key_Backslash = 92,  /* \ */
		Key_RightBracket = 93,  /* ] */
		Key_GraveAccent = 96,  /* ` */
		Key_World1 = 161, /* non-US #1 */
		Key_World2 = 162, /* non-US #2 */
		
		/* Function keys */
		Key_Escape = 256,
		Key_Enter = 257,
		Key_Tab = 258,
		Key_Backspace = 259,
		Key_Insert = 260,
		Key_Delete = 261,
		Key_Right = 262,
		Key_Left = 263,
		Key_Down = 264,
		Key_Up = 265,
		Key_Page_Up = 266,
		Key_Page_Down = 267,
		Key_Home = 268,
		Key_End = 269,
		Key_Caps_Lock = 280,
		Key_Scroll_Lock = 281,
		Key_Num_Lock = 282,
		Key_Print_Screen = 283,
		Key_Pause = 284,
		Key_F1 = 290,
		Key_F2 = 291,
		Key_F3 = 292,
		Key_F4 = 293,
		Key_F5 = 294,
		Key_F6 = 295,
		Key_F7 = 296,
		Key_F8 = 297,
		Key_F9 = 298,
		Key_F10 = 299,
		Key_F11 = 300,
		Key_F12 = 301,
		Key_F13 = 302,
		Key_F14 = 303,
		Key_F15 = 304,
		Key_F16 = 305,
		Key_F17 = 306,
		Key_F18 = 307,
		Key_F19 = 308,
		Key_F20 = 309,
		Key_F21 = 310,
		Key_F22 = 311,
		Key_F23 = 312,
		Key_F24 = 313,
		Key_F25 = 314,
		Key_Keypad0 = 320,
		Key_Keypad1 = 321,
		Key_Keypad2 = 322,
		Key_Keypad3 = 323,
		Key_Keypad4 = 324,
		Key_Keypad5 = 325,
		Key_Keypad6 = 326,
		Key_Keypad7 = 327,
		Key_Keypad8 = 328,
		Key_Keypad9 = 329,
		Key_KeypadDecimal = 330,
		Key_KeypadDivide = 331,
		Key_KeypadMultiply = 332,
		Key_KeypadSubtract = 333,
		Key_KeypadAdd = 334,
		Key_KeypadEnter = 335,
		Key_KeypadEqual = 336,
		Key_LeftShift = 340,
		Key_LeftControl = 341,
		Key_LeftAlt = 342,
		Key_LeftSuper = 343,
		Key_RightShift = 344,
		Key_RightControl = 345,
		Key_RightAlt = 346,
		Key_RightSuper = 347,
		Key_Menu = 348
	} Key;
	
	
//	typedef enum {
//		ModifierKey_Shift = 0x0001,
//		ModifierKey_Control = 0x0002,
//		ModifierKey_Alt = 0x0004,
//		ModifierKey_Super = 0x0008
//	} ModifierKey;
	
	
	typedef enum {
		MouseButton_1 = 0,
		MouseButton_2 = 1,
		MouseButton_3 = 2,
		MouseButton_4 = 3,
		MouseButton_5 = 4,
		MouseButton_6 = 5,
		MouseButton_7 = 6,
		MouseButton_8 = 7
	} MouseButton;
}


#endif /* Types_h */
