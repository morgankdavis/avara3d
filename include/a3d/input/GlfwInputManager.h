//
//  WindowInputManager.h
//  avara3d
//
//  Created by Morgan Davis on 4/16/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_GLFWINPUTMANAGER_H
#define AVARA3D_GLFWINPUTMANAGER_H


#include <memory>
#include <set>

#include "glm/vec2.hpp"

#include "a3d/Types.h"
#include "a3d/input/DesktopInputManager.h"


struct GLFWwindow;


namespace a3d {


	class GlfwWindow;

	
	class GlfwInputManager : public DesktopInputManager {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit GlfwInputManager(GlfwWindow* window);
		GlfwInputManager(const InputManager& other) = delete; // copy constructor
		GlfwInputManager& operator=(const InputManager& other) = delete; // copy assignment
		~GlfwInputManager() override;

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

//		DesktopInputManagerErrorMask		errorMask() const;

/*********************************************************************************************
	InputManager Internal Member Functions
 *********************************************************************************************/

		void 							update() override;

/*********************************************************************************************
	Private Member Functions
 *********************************************************************************************/

//		void 							initMouseInput();

	private:

//		void 							initManyMouse();
//		void 							quitManyMouse();
		void 							registerGLFWCallbacks(GLFWwindow* glfwWindow);
		void 							unregisterGLFWCallbacks(GLFWwindow* glfwWindow);

/*********************************************************************************************
	Private Static Member Functions
 *********************************************************************************************/

		static void 					GLFWMouseButtonCallback(GLFWwindow* glfwWindow,
															   int button,
															   int action,
															   int mods);
		static void 					GLFWCursorPositionCallback(GLFWwindow* glfwWindow,
																  double xPos,
																  double yPos);
		static void 					GLFWScrollWheelCallback(GLFWwindow* glfwWindow,
															   double xOffset,
															   double yOffset);
		static void 					GLFWKeyCallback(GLFWwindow* glfwWindow,
													   int key,
													   int scancode,
													   int action,
													   int mods);

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

//		bool							_usingManyMouse;
		GlfwWindow*						_window;
//		DesktopInputManagerErrorMask	_errorMask;
	};
}


#endif /* AVARA3D_GLFWINPUTMANAGER_H */
