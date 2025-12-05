//
//  QtInputManager.h
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_QTINPUTMANAGER_H
#define AVARA3D_QTINPUTMANAGER_H

#include "a3d/input/DesktopInputManager.h"

namespace a3d::head::qt {

	class QtViewport;

	class QtInputManager : public a3d::DesktopInputManager {

	public:
		/// Public Lifecycle Functions ///

		explicit QtInputManager(QtViewport& viewport);

		/// Internal Member Functions ///

		void 	keyPressed(int qtKey);
		void 	keyReleased(int qtKey);
		void 	mouseMoved(float x, float y);
		void	mouseButtonPressed(int qtButton);
		void	mouseButtonReleased(int qtButton);
		void	mouseWheelScrolled(int x, int y);

		/// InputManager Internal Member Functions ///

		void	update() override;
	};
}

#endif //AVARA3D_QTINPUTMANAGER_H
