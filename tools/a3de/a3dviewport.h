#ifndef A3DVIEWPORT_H
#define A3DVIEWPORT_H

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include "a3d/rendering/context/RenderContext.h"

namespace a3d {

	class Scene;
}

namespace a3de {

	class A3DViewport : public QOpenGLWidget, public a3d::RenderContext {
            //protected QOpenGLFunctions_3_3_Core

	Q_OBJECT

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit A3DViewport(a3d::RenderingApi renderingApi, QWidget* parent = nullptr);
		~A3DViewport() override;

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		a3d::Scene*		scene() const;
		void 			scene(a3d::Scene* scene);

/*********************************************************************************************
	RenderContext Public Member Functions
 *********************************************************************************************/

		bool 			vSyncEnabled() const override;
		void 			vSyncEnabled(bool enabled) override;

/*********************************************************************************************
	RenderContext Internal Member Functions
 *********************************************************************************************/

		void 			beginFrame(const a3d::Scene& scene) override;
		void 			endFrame(const a3d::Scene& scene) override;

		void 			swapBuffers() override;

		glm::uvec2 		framebufferSize() const override;
		glm::vec2 		framebufferScale() const override;

		unsigned 		defaultFramebuffer() const override;

/*********************************************************************************************
	QOpenGLWidget Protected Member Functions
 *********************************************************************************************/

	protected:

		void 			initializeGL() override;
		void 			resizeGL(int w, int h) override;
		void 			paintGL() override;

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		a3d::Scene* 	_scene;
	};
}

#endif // A3DVIEWPORT_H
