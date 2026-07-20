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

namespace a3d::qt {

	class QtViewport;

	class QtInputManager : public DesktopInputManager {

	public:
		/// Public Lifecycle Functions ///

		// explicit QtInputManager(QtViewport& viewport);
		QtInputManager();

		/// Internal Member Functions ///

		void 			keyPressed(int qtKey, int modifiers);
		void 			keyReleased(int qtKey, int modifiers);
		void 			mouseMoved(float x, float y);
		void			mouseButtonPressed(int qtButton);
		void			mouseButtonReleased(int qtButton);
		void			mouseWheelScrolled(int x, int y);

		/// InputManager Internal Member Functions ///

		void			update() override;
		void			attachedToScene(Scene& scene) override;
		void			visualWorldAttachedToScene(Scene& scene) override;

	private:
		/// Private Member Functions ///

		void			viewport(QtViewport* viewport);
		QtViewport*		viewport() const;

		/// Private Member Variables ///

		QtViewport*		_viewport;
	};
}

#endif //AVARA3D_QTINPUTMANAGER_H
