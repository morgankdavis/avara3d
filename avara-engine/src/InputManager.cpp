////
////  InputManager.cpp
////	avara-engine
////
////  Created by Morgan Davis on 10/9/17.
////  Copyright © 2017 Morgan K Davis. All rights reserved.
////
//
//#include "InputManager.h"
//
//#include <iostream>
//
//#define GLFW_DLL
//#include <GLFW/glfw3.h>
//
//#include "Globals.h"
//#include "Window.h"
//
//
//using namespace ae;
//using namespace std;
//using namespace glm;
//
//
///***************************************************************************************
//     MARK:   Globals
// **************************************************************************************/
//
//InputManager *inputManager;
//
///***************************************************************************************
//     MARK:   GLFW Callbacks
// **************************************************************************************/
//
//void glfwMouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
//	cout << "glfwMouseButtonCallback()" << endl;
//}
//
//void glfwCursorPositionCallback(GLFWwindow* glfwWindow, double xPos, double yPos) {
//	cout << "glfwCursorPositionCallback()" << endl;
//}
//
//void glfwScrollWheelCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset) {
//	cout << "glfwScrollWheelCallback()" << endl;
//}
//
//void glfwKeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
//	cout << "glfwKeyCallback()" << endl;
//
//
//	//const char* keyName = glfwGetKeyName(key, scancode);
//}
//
///***************************************************************************************
//     MARK:   Lifecycle
// **************************************************************************************/
//
//InputManager::InputManager(const shared_ptr<Window> window):
//		m_keysDown(vector<KeyCode>()),
//		m_mouseButtonsDown(vector<MouseButtonCode>()),
//		m_mouseMoveDelta(vec2(0.0f, 0.0f)),
//		m_mouseScrollWheelDelta(vec2(0.0f, 0.0f)),
//		m_window(window) {
//
//	inputManager = this;
//
//
//	// register GLFW callbacks
//
//	// TODO: USE A LOCAL GLFEWINDOW FOR WINDOW!
//	GLFWwindow* glfwWindow = g_glfwWindow;
//
//	glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
//	glfwSetCursorPosCallback(glfwWindow, glfwCursorPositionCallback);
//	glfwSetScrollCallback(glfwWindow, glfwScrollWheelCallback);
//	glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
//}
//
///***************************************************************************************
//     MARK:   Public
// **************************************************************************************/
//
//map<KeyCode, float> InputManager::keysDown() {
//
//}
//
//map<MouseButtonCode, float> InputManager::mouseButtonsDown() {
//
//}
//
//vec2 InputManager::mouseMoveDelta() {
//
//}
//
//vec2 InputManager::mouseScrollWheelDelta() {
//
//}
//
//shared_ptr<Window> InputManager::window() const {
//	return m_window;
//}
//
///***************************************************************************************
//     MARK:   Private
// **************************************************************************************/
//
//void InputManager::clearKeysDown() {
//
//}
//
//void InputManager::clearMouseDown() {
//
//}
//
//void InputManager::clearMouseMoveDelta() {
//
//}
//
//void InputManager::clearMouseScrollWheelDelta() {
//
//}

