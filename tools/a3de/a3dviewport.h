#ifndef A3DVIEWPORT_H
#define A3DVIEWPORT_H

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include "a3d/rendering/context/RenderContext.h"

namespace a3de {

	class A3DViewport : public QOpenGLWidget, public a3d::RenderContext {
            //protected QOpenGLFunctions_3_3_Core

	Q_OBJECT

	public:

		a3d::Scene* scene;


		A3DViewport(a3d::RenderingApi renderingApi, QWidget* parent = nullptr);
		~A3DViewport() override;

		void swapBuffers() override;

		glm::uvec2 framebufferSize() const override;
		glm::vec2 framebufferScale() const override;

		unsigned defaultFramebuffer() const override;

	protected:

		void initializeGL() override;
		void resizeGL(int w, int h) override;
		void paintGL() override;

	private:

		//a3d::Scene* _scene;
		int _width, _height;
	};
}

#endif // A3DVIEWPORT_H
