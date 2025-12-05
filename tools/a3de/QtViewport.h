//
//  QtViewport.h
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef A3DVIEWPORT_H
#define A3DVIEWPORT_H

#include <memory>

#include <QOpenGLWidget>

#include "a3d/rendering/context/RenderContext.h"

namespace a3d {
	class Scene;
}

namespace a3d::head::qt {

	class QtInputManager;

	class QtViewport : public QOpenGLWidget, public a3d::RenderContext {

	Q_OBJECT

	public:
		/// Public Lifecycle Functions ///

		explicit QtViewport(a3d::RenderingApi renderingApi, QWidget* parent = nullptr);
		~QtViewport() override;

		/// Public Member Functions ///

		a3d::Scene*				scene() const;
		void 					scene(a3d::Scene* scene);

		bool 					cursorCaptured() const;
		void 					cursorCaptured(bool captured);

	public:
		/// Public Member Functions ///

		void					inputManager(QtInputManager* manager);

		/// RenderContext Public Member Functions ///

		bool 					vSyncEnabled() const override;
		void 					vSyncEnabled(bool enabled) override;

		/// RenderContext Internal Member Functions ///

		void 					beginFrame(const a3d::Scene& scene) override;
		void 					endFrame(const a3d::Scene& scene) override;

		void 					swapBuffers() override;

		glm::uvec2 				framebufferSize() const override;
		glm::vec2 				framebufferScale() const override;

		unsigned 				defaultFramebuffer() const override;

	protected:
		/// QWidget Protected Member Functions ///

		bool 					event(QEvent* e) override;
		void 					keyPressEvent(QKeyEvent* e) override;
		void 					keyReleaseEvent(QKeyEvent* e) override;
		void 					mouseMoveEvent(QMouseEvent *e) override;

		/// QOpenGLWidget Protected Member Functions ///

		void 					initializeGL() override;
		void 					resizeGL(int w, int h) override;
		void 					paintGL() override;

	private:
		/// Private Member Functions ///

		void					centerCursor();

		/// Private Member Variables ///

		a3d::Scene* 			_scene;
		bool					_cursorCaptured;
		std::optional<QPointF> 	_lastCapturedCursorPosition;
		QtInputManager*			_inputManager;
		bool 					_warpingCursor;
	};
}

#endif // A3DVIEWPORT_H
