
#include "QtInputManager.h"

#include <QEvent>
#include <QMouseEvent>

#include "magic_enum.hpp"

#include "a3d/diagnostic/log/Log.h"

#include "a3dviewport.h"


using namespace a3de;


/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/
A3DViewport* vp;
QtInputManager::QtInputManager(A3DViewport& viewport) {
	viewport.inputManager(this);
	vp = &viewport;
}

/*********************************************************************************************
 	Internal Member Functions
 *********************************************************************************************/

void QtInputManager::event(QEvent* e) {


}

void QtInputManager::keyPressed(int key) {
	A3D_LOG_I("KEY: {}", key);

	if (key == 47) {

		//cursorCaptured(!_cursorCaptured);
		vp->cursorCaptured(!vp->cursorCaptured());
	}
	else if (key == 16777216) {
		QCoreApplication::quit();
	}
}

void QtInputManager::keyReleased(int key) {
	A3D_LOG_I("KEY: {}", key);
}

void QtInputManager::mouseMoved(QPointF delta) {

//	QEvent::Type type = e->type();
	A3D_LOG_I("delta: ({}, {})", delta.x(), delta.y());//magic_enum::enum_name<QEvent::Type>(type));
}

/*********************************************************************************************
	InputManager Internal Member Functions
 *********************************************************************************************/

void QtInputManager::update() {}
