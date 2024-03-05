//
//  ActivityInputManager.cc
//	avara3d
//
//  Created by Morgan Davis on 5/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifdef ANDROID


#include "a3d/ActivityInputManager.h"

#include <android_native_app_glue.h>
#include <android/native_window_jni.h>

#include "a3d/Activity.h"
#include "a3d/Exception.h"
#include "a3d/Logger.h"
#include "a3d/Utilities.h"


using namespace a3d;
using namespace a3d::utils;
using namespace std;
using namespace glm;


#define FLIP_MOUSE_VERTICAL		true
#define FLIP_MOUSE_HORIZONTAL	false


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static KEY A3DKeyForChromeOSKeyCode(int32_t code);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

ActivityInputManager::ActivityInputManager(shared_ptr<Activity> activity):
	InputManager(),
	//_previousMouseButtonsDown(set<MOUSE_BUTTON>()),
	_activity(activity) {

}

ActivityInputManager::~ActivityInputManager() {

}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

int ActivityInputManager::update(AInputEvent* event) {
	int32_t eventType = AInputEvent_getType(event);
	
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		int32_t action = AMotionEvent_getAction(event);
		
		switch (action) {
				
			case AMOTION_EVENT_ACTION_BUTTON_PRESS: {
				A3D_LOG_T("AMOTION_EVENT_ACTION_BUTTON_PRESS");
				
				int32_t buttonStates = AMotionEvent_getButtonState(event);
				
				if (buttonStates & AMOTION_EVENT_BUTTON_PRIMARY) {
					mouseButton(MOUSE_BUTTON::ONE, true);
				}
				
				if (buttonStates & AMOTION_EVENT_BUTTON_SECONDARY) {
					mouseButton(MOUSE_BUTTON::TWO, true);
				}
				
				if (buttonStates & AMOTION_EVENT_BUTTON_TERTIARY) {
					mouseButton(MOUSE_BUTTON::THREE, true);
				}

				break; }
				
			case AMOTION_EVENT_ACTION_BUTTON_RELEASE: {
				A3D_LOG_T("AMOTION_EVENT_ACTION_BUTTON_RELEASE");

				int32_t buttonStates = AMotionEvent_getButtonState(event);

				if (_mouseButtonsDown.count(MOUSE_BUTTON::ONE)
					&& !(buttonStates & AMOTION_EVENT_BUTTON_PRIMARY)) {
					mouseButton(MOUSE_BUTTON::ONE, false);
				}
				
				if (_mouseButtonsDown.count(MOUSE_BUTTON::TWO)
					&& !(buttonStates & AMOTION_EVENT_BUTTON_SECONDARY)) {
					mouseButton(MOUSE_BUTTON::TWO, false);
				}
				
				if (_mouseButtonsDown.count(MOUSE_BUTTON::THREE)
					&& !(buttonStates & AMOTION_EVENT_BUTTON_TERTIARY)) {
					mouseButton(MOUSE_BUTTON::THREE, false);
				}
				
				break; }

			case AMOTION_EVENT_ACTION_DOWN: {
				A3D_LOG_T("AMOTION_EVENT_ACTION_DOWN");
				break; }
				
			case AMOTION_EVENT_ACTION_UP:
				A3D_LOG_T("AMOTION_EVENT_ACTION_UP");
				break;
				
			case AMOTION_EVENT_ACTION_HOVER_MOVE: {
			case AMOTION_EVENT_ACTION_MOVE:
				A3D_LOG_T("AMOTION_EVENT_ACTION_MOVE");
				
//				float rawX = AMotionEvent_getRawX(event, 0);
//				float rawY = AMotionEvent_getRawY(event, 0);
//				float xOffset = AMotionEvent_getXOffset(event);
//				float yOffset = AMotionEvent_getYOffset(event);
				float xPos = AMotionEvent_getX(event, 0);
				float yPos = AMotionEvent_getY(event, 0);
				float xDelta = 0;
				float yDelta = 0;
				static float oldXPos = numeric_limits<float>::max();
				static float oldYPos = numeric_limits<float>::max();
				
				if (oldXPos == numeric_limits<float>::max()) {
					oldXPos = xPos;
				}
				else {
					xDelta = xPos - oldXPos;
				}
				if (oldYPos == numeric_limits<float>::max()) {
					oldYPos = yPos;
				}
				else {
					yDelta = yPos - oldYPos;
				}
				
				oldXPos = xPos;
				oldYPos = yPos;
				
				_mousePositionDelta.x = (FLIP_MOUSE_HORIZONTAL ? -xDelta : xDelta);
				_mousePositionDelta.y = (FLIP_MOUSE_VERTICAL ? -yDelta : yDelta);
				
				break; }
				
			case AMOTION_EVENT_ACTION_SCROLL: {
				A3D_LOG_T("AMOTION_EVENT_ACTION_SCROLL");
				
				float hScroll = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HSCROLL, 0);
				float vScroll = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_VSCROLL, 0);

				if (!Equal(vScroll, 0.0)) {
					if (vScroll > 0.0) _mouseScrollWheelDelta.y += 1;
					else _mouseScrollWheelDelta.y -= 1;
				}
				
				if (!Equal(hScroll, 0.0)) {
					if (vScroll > 0.0) _mouseScrollWheelDelta.x += 1;
					else _mouseScrollWheelDelta.x -= 1;
				}
				
				break; }
				
			default:
				//A3D_LOG_I("[unknown]");
				break;
		}
	}
	else if (eventType == AINPUT_EVENT_TYPE_KEY) {
		
		int32_t keyCode = AKeyEvent_getKeyCode(event);
		int32_t action = AMotionEvent_getAction(event);
		auto key = A3DKeyForChromeOSKeyCode(keyCode);
		
		if (action == AMOTION_EVENT_ACTION_DOWN) {
			_keysDown.insert(key);
			
			// if key is in "cleared" it means the client already read it, so don't add it again until
			// we get key up, and then back down again
			if (_keysPressedCleared.count(key) == 0) {
				_keysPressed.insert(key);
			}
		}
		else if (action == AMOTION_EVENT_ACTION_UP) {
			_keysDown.erase(key);
			_keysPressedCleared.erase(key);
		}
	}
	
	return 0;
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

void ActivityInputManager::mouseButton(MOUSE_BUTTON button, bool down) {

	if (down) {
		_mouseButtonsDown.insert(button);
		
		// if button is in "cleared" it means the client already read it, so don't add it again until
		// we get button up, and then back down again
		if (_mouseButtonsPressedCleared.count(button) == 0) {
			_mouseButtonsPressed.insert(button);
		}
	}
	else {
		_mouseButtonsDown.erase(button);
		_mouseButtonsPressedCleared.erase(button);
	}
}

/*********************************************************************************************
	Static
 *********************************************************************************************/

static KEY A3DKeyForChromeOSKeyCode(int32_t code) {
	
	if (code >= AKEYCODE_A && code <= AKEYCODE_Z) {
		return static_cast<KEY>(code + 36);
	}
	
	if (code >= AKEYCODE_0 && code <= AKEYCODE_9) {
		return static_cast<KEY>(code + 41);
	}
	
	if (code >= AKEYCODE_F1 && code <= AKEYCODE_F12) {
		return static_cast<KEY>(code + 156);
	}
	
	if (code >= AKEYCODE_NUMPAD_0 && code <= AKEYCODE_NUMPAD_9) {
		return static_cast<KEY>(code + 176);
	}
	
	switch (code) {
		case AKEYCODE_NUMPAD_DIVIDE: 	return KEY::KEYPAD_DIVIDE;
		case AKEYCODE_NUMPAD_MULTIPLY: 	return KEY::KEYPAD_MULTIPLY;
		case AKEYCODE_NUMPAD_SUBTRACT: 	return KEY::KEYPAD_SUBTRACT;
		case AKEYCODE_NUMPAD_ADD: 		return KEY::KEYPAD_ADD;
		case AKEYCODE_NUMPAD_DOT: 		return KEY::KEYPAD_DECIMAL;
		case AKEYCODE_NUMPAD_ENTER: 	return KEY::KEYPAD_ENTER;
		case AKEYCODE_NUMPAD_EQUALS: 	return KEY::KEYPAD_EQUAL;
		case AKEYCODE_ALT_LEFT: 		return KEY::LEFT_ALT;
		case AKEYCODE_ALT_RIGHT: 		return KEY::RIGHT_ALT;
		case AKEYCODE_SHIFT_LEFT: 		return KEY::LEFT_SHIFT;
		case AKEYCODE_SHIFT_RIGHT: 		return KEY::RIGHT_SHIFT;
		case AKEYCODE_CTRL_LEFT: 		return KEY::LEFT_CONTROL;
		case AKEYCODE_CTRL_RIGHT: 		return KEY::RIGHT_CONTROL;
		case AKEYCODE_COMMA: 			return KEY::COMMA;
		case AKEYCODE_PERIOD: 			return KEY::PERIOD;
		case AKEYCODE_TAB: 				return KEY::TAB;
		case AKEYCODE_SPACE: 			return KEY::SPACE;
		case AKEYCODE_ENTER: 			return KEY::ENTER;
		case AKEYCODE_DEL: 				return KEY::BACKSPACE;
		case AKEYCODE_MINUS: 			return KEY::MINUS;
		case AKEYCODE_EQUALS: 			return KEY::EQUAL;
		case AKEYCODE_LEFT_BRACKET: 	return KEY::LEFT_BRACKET;
		case AKEYCODE_RIGHT_BRACKET: 	return KEY::RIGHT_BRACKET;
		case AKEYCODE_BACKSLASH: 		return KEY::BACKSLASH;
		case AKEYCODE_SEMICOLON: 		return KEY::SEMICOLON;
		case AKEYCODE_APOSTROPHE: 		return KEY::APOSTROPHE;
		case AKEYCODE_SLASH: 			return KEY::SLASH;
		case AKEYCODE_GRAVE: 			return KEY::GRAVE_ACCENT;
		case AKEYCODE_ESCAPE: 			return KEY::ESCAPE;
		case AKEYCODE_INSERT: 			return KEY::INSERT;
		case AKEYCODE_FORWARD_DEL: 		return KEY::FORWARD_DELETE;
		case AKEYCODE_DPAD_DOWN: 		return KEY::DOWN;
		case AKEYCODE_DPAD_LEFT: 		return KEY::LEFT;
		case AKEYCODE_DPAD_RIGHT: 		return KEY::RIGHT;
		case AKEYCODE_DPAD_UP: 			return KEY::UP;
		case AKEYCODE_PAGE_UP: 			return KEY::PAGE_UP;
		case AKEYCODE_PAGE_DOWN: 		return KEY::PAGE_DOWN;
		case AKEYCODE_MOVE_HOME: 		return KEY::HOME;
		case AKEYCODE_MOVE_END: 		return KEY::END;
		case AKEYCODE_CAPS_LOCK: 		return KEY::CAPS_LOCK;
		case AKEYCODE_SCROLL_LOCK: 		return KEY::SCROLL_LOCK;
		case AKEYCODE_NUM_LOCK: 		return KEY::NUM_LOCK;
		case AKEYCODE_MENU: 			return KEY::MENU;
		case AKEYCODE_MEDIA_PAUSE: 		return KEY::PAUSE;
		default: 						return KEY::UNKNOWN;
	}
	
	// missing AE:KEYs
	//
	//	LEFT_SUPER = 343,
	//	RIGHT_SUPER = 347
	//	WORLD_1 = 161, /* non-US #1 */
	//	WORLD_2 = 162, /* non-US #2 */
	//	PRINT_SCREEN = 283,
	
	// missing ChromeOS keys:
	//	
	//	AKEYCODE_UNKNOWN = 0,
	//	AKEYCODE_SOFT_LEFT = 1,
	//	AKEYCODE_SOFT_RIGHT = 2,
	//	AKEYCODE_HOME = 3,
	//	AKEYCODE_BACK = 4,
	//	AKEYCODE_CALL = 5,
	//	AKEYCODE_ENDCALL = 6,
	//	  AKEYCODE_NUMPAD_LEFT_PAREN = 162,
	//	AKEYCODE_NUMPAD_RIGHT_PAREN = 163,
	//	AKEYCODE_VOLUME_MUTE = 164,
	//	AKEYCODE_INFO = 165,
	//	AKEYCODE_CHANNEL_UP = 166,
	//	AKEYCODE_CHANNEL_DOWN = 167,
	//	AKEYCODE_ZOOM_IN = 168,
	//	AKEYCODE_ZOOM_OUT = 169,
	//	AKEYCODE_TV = 170,
	//	AKEYCODE_WINDOW = 171,
	//	AKEYCODE_GUIDE = 172,
	//	AKEYCODE_DVR = 173,
	//	AKEYCODE_BOOKMARK = 174,
	//	AKEYCODE_CAPTIONS = 175,
	//	AKEYCODE_SETTINGS = 176,
	//	AKEYCODE_TV_POWER = 177,
	//	AKEYCODE_TV_INPUT = 178,
	//	AKEYCODE_STB_POWER = 179,
	//	AKEYCODE_STB_INPUT = 180,
	//	AKEYCODE_AVR_POWER = 181,
	//	AKEYCODE_AVR_INPUT = 182,
	//	AKEYCODE_PROG_RED = 183,
	//	AKEYCODE_PROG_GREEN = 184,
	//	AKEYCODE_PROG_YELLOW = 185,
	//	AKEYCODE_PROG_BLUE = 186,
	//	AKEYCODE_APP_SWITCH = 187,
	//	AKEYCODE_BUTTON_1 = 188,
	//	AKEYCODE_BUTTON_2 = 189,
	//	AKEYCODE_BUTTON_3 = 190,
	//	AKEYCODE_BUTTON_4 = 191,
	//	AKEYCODE_BUTTON_5 = 192,
	//	AKEYCODE_BUTTON_6 = 193,
	//	AKEYCODE_BUTTON_7 = 194,
	//	AKEYCODE_BUTTON_8 = 195,
	//	AKEYCODE_BUTTON_9 = 196,
	//	AKEYCODE_BUTTON_10 = 197,
	//	AKEYCODE_BUTTON_11 = 198,
	//	AKEYCODE_BUTTON_12 = 199,
	//	AKEYCODE_BUTTON_13 = 200,
	//	AKEYCODE_BUTTON_14 = 201,
	//	AKEYCODE_BUTTON_15 = 202,
	//	AKEYCODE_BUTTON_16 = 203,
	//	AKEYCODE_LANGUAGE_SWITCH = 204,
	//	AKEYCODE_MANNER_MODE = 205,
	//	AKEYCODE_3D_MODE = 206,
	//	AKEYCODE_CONTACTS = 207,
	//	AKEYCODE_CALENDAR = 208,
	//	AKEYCODE_MUSIC = 209,
	//	AKEYCODE_CALCULATOR = 210,
	//	AKEYCODE_ZENKAKU_HANKAKU = 211,
	//	AKEYCODE_EISU = 212,
	//	AKEYCODE_MUHENKAN = 213,
	//	AKEYCODE_HENKAN = 214,
	//	AKEYCODE_KATAKANA_HIRAGANA = 215,
	//	AKEYCODE_YEN = 216,
	//	AKEYCODE_RO = 217,
	//	AKEYCODE_KANA = 218,
	//	AKEYCODE_ASSIST = 219,
	//	AKEYCODE_BRIGHTNESS_DOWN = 220,
	//	AKEYCODE_BRIGHTNESS_UP = 221,
	//	AKEYCODE_MEDIA_AUDIO_TRACK = 222,
	//	AKEYCODE_SLEEP = 223,
	//	AKEYCODE_WAKEUP = 224,
	//	AKEYCODE_PAIRING = 225,
	//	AKEYCODE_MEDIA_TOP_MENU = 226,
	//	AKEYCODE_11 = 227,
	//	AKEYCODE_12 = 228,
	//	AKEYCODE_LAST_CHANNEL = 229,
	//	AKEYCODE_TV_DATA_SERVICE = 230,
	//	AKEYCODE_VOICE_ASSIST = 231,
	//	AKEYCODE_TV_RADIO_SERVICE = 232,
	//	AKEYCODE_TV_TELETEXT = 233,
	//	AKEYCODE_TV_NUMBER_ENTRY = 234,
	//	AKEYCODE_TV_TERRESTRIAL_ANALOG = 235,
	//	AKEYCODE_TV_TERRESTRIAL_DIGITAL = 236,
	//	AKEYCODE_TV_SATELLITE = 237,
	//	AKEYCODE_TV_SATELLITE_BS = 238,
	//	AKEYCODE_TV_SATELLITE_CS = 239,
	//	AKEYCODE_TV_SATELLITE_SERVICE = 240,
	//	AKEYCODE_TV_NETWORK = 241,
	//	AKEYCODE_TV_ANTENNA_CABLE = 242,
	//	AKEYCODE_TV_INPUT_HDMI_1 = 243,
	//	AKEYCODE_TV_INPUT_HDMI_2 = 244,
	//	AKEYCODE_TV_INPUT_HDMI_3 = 245,
	//	AKEYCODE_TV_INPUT_HDMI_4 = 246,
	//	AKEYCODE_TV_INPUT_COMPOSITE_1 = 247,
	//	AKEYCODE_TV_INPUT_COMPOSITE_2 = 248,
	//	AKEYCODE_TV_INPUT_COMPONENT_1 = 249,
	//	AKEYCODE_TV_INPUT_COMPONENT_2 = 250,
	//	AKEYCODE_TV_INPUT_VGA_1 = 251,
	//	AKEYCODE_TV_AUDIO_DESCRIPTION = 252,
	//	AKEYCODE_TV_AUDIO_DESCRIPTION_MIX_UP = 253,
	//	AKEYCODE_TV_AUDIO_DESCRIPTION_MIX_DOWN = 254,
	//	AKEYCODE_TV_ZOOM_MODE = 255,
	//	AKEYCODE_TV_CONTENTS_MENU = 256,
	//	AKEYCODE_TV_MEDIA_CONTEXT_MENU = 257,
	//	AKEYCODE_TV_TIMER_PROGRAMMING = 258,
	//	AKEYCODE_HELP = 259,
	//	AKEYCODE_NAVIGATE_PREVIOUS = 260,
	//	AKEYCODE_NAVIGATE_NEXT = 261,
	//	AKEYCODE_NAVIGATE_IN = 262,
	//	AKEYCODE_NAVIGATE_OUT = 263,
	//	AKEYCODE_STEM_PRIMARY = 264,
	//	AKEYCODE_STEM_1 = 265,
	//	AKEYCODE_STEM_2 = 266,
	//	AKEYCODE_STEM_3 = 267,
	//	AKEYCODE_DPAD_UP_LEFT = 268,
	//	AKEYCODE_DPAD_DOWN_LEFT = 269,
	//	AKEYCODE_DPAD_UP_RIGHT = 270,
	//	AKEYCODE_DPAD_DOWN_RIGHT = 271,
	//	AKEYCODE_MEDIA_SKIP_FORWARD = 272,
	//	AKEYCODE_MEDIA_SKIP_BACKWARD = 273,
	//	AKEYCODE_MEDIA_STEP_FORWARD = 274,
	//	AKEYCODE_MEDIA_STEP_BACKWARD = 275,
	//	AKEYCODE_SOFT_SLEEP = 276,
	//	AKEYCODE_CUT = 277,
	//	AKEYCODE_COPY = 278,
	//	AKEYCODE_PASTE = 279,
	//	AKEYCODE_SYSTEM_NAVIGATION_UP = 280,
	//	AKEYCODE_SYSTEM_NAVIGATION_DOWN = 281,
	//	AKEYCODE_SYSTEM_NAVIGATION_LEFT = 282,
	//	AKEYCODE_SYSTEM_NAVIGATION_RIGHT = 283,
	//	AKEYCODE_ALL_APPS = 284,
	//	AKEYCODE_REFRESH = 285
	//	 AKEYCODE_FORWARD = 125,
	//	AKEYCODE_MEDIA_PLAY = 126,
	//	AKEYCODE_MEDIA_CLOSE = 128,
	//	AKEYCODE_MEDIA_EJECT = 129,
	//	AKEYCODE_MEDIA_RECORD = 130,
	//	AKEYCODE_PICTSYMBOLS = 94,
	//	AKEYCODE_SWITCH_CHARSET = 95,
	//	AKEYCODE_BUTTON_A = 96,
	//	AKEYCODE_BUTTON_B = 97,
	//	AKEYCODE_BUTTON_C = 98,
	//	AKEYCODE_BUTTON_X = 99,
	//	AKEYCODE_BUTTON_Y = 100,
	//	AKEYCODE_BUTTON_Z = 101,
	//	AKEYCODE_BUTTON_L1 = 102,
	//	AKEYCODE_BUTTON_R1 = 103,
	//	AKEYCODE_BUTTON_L2 = 104,
	//	AKEYCODE_BUTTON_R2 = 105,
	//	AKEYCODE_BUTTON_THUMBL = 106,
	//	AKEYCODE_BUTTON_THUMBR = 107,
	//	AKEYCODE_BUTTON_START = 108,
	//	AKEYCODE_BUTTON_SELECT = 109,
	//	AKEYCODE_BUTTON_MODE = 110,
	//	AKEYCODE_STAR = 17,
	//	AKEYCODE_POUND = 18,
	//	AKEYCODE_DPAD_CENTER = 23,
	//	AKEYCODE_VOLUME_UP = 24,
	//	AKEYCODE_VOLUME_DOWN = 25,
	//	AKEYCODE_POWER = 26,
	//	AKEYCODE_CAMERA = 27,
	//	AKEYCODE_CLEAR = 28,
	//	AKEYCODE_SYM = 63,
	//	AKEYCODE_EXPLORER = 64,
	//	AKEYCODE_ENVELOPE = 65,
	//	AKEYCODE_META_LEFT = 117,
	//	AKEYCODE_META_RIGHT = 118,
	//	AKEYCODE_FUNCTION = 119,
	//	AKEYCODE_SYSRQ = 120,
	//	AKEYCODE_BREAK = 121,
	//	AKEYCODE_AT = 77,
	//	AKEYCODE_NUM = 78,
	//	AKEYCODE_HEADSETHOOK = 79,
	//	AKEYCODE_FOCUS = 80,
	//	AKEYCODE_PLUS = 81,
	//	AKEYCODE_MENU = 82,
	//	AKEYCODE_NOTIFICATION = 83,
	//	AKEYCODE_SEARCH = 84,
	//	AKEYCODE_MEDIA_PLAY_PAUSE = 85,
	//	AKEYCODE_MEDIA_STOP = 86,
	//	AKEYCODE_MEDIA_NEXT = 87,
	//	AKEYCODE_MEDIA_PREVIOUS = 88,
	//	AKEYCODE_MEDIA_REWIND = 89,
	//	AKEYCODE_MEDIA_FAST_FORWARD = 90,
	//	AKEYCODE_MUTE = 91,
	//	AKEYCODE_NUMPAD_COMMA = 159,
}

#endif // ANDROID
