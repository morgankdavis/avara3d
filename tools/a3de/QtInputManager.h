
#ifndef AVARA3D_QTINPUTMANAGER_H
#define AVARA3D_QTINPUTMANAGER_H


#include "a3d/input/DesktopInputManager.h"


class QEvent;
//class QKeyEvent;
//class QMouseEvent;


//namespace a3de {
//	class A3DViewport;
//}
class QPointF;


namespace a3de {


	class A3DViewport;


	class QtInputManager : public a3d::DesktopInputManager {

	public:

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

		explicit QtInputManager(A3DViewport& viewport);

/*********************************************************************************************
 	Internal Member Functions
 *********************************************************************************************/

		void	event(QEvent* e);

		// mouse button
		// mouse wheel

		void 	keyPressed(int key);
		void 	keyReleased(int key);
		void 	mouseMoved(QPointF delta);

/*********************************************************************************************
	InputManager Internal Member Functions
 *********************************************************************************************/

		void	update() override;
	};
}


#endif //AVARA3D_QTINPUTMANAGER_H
