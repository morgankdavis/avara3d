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

		A3DViewport(a3d::RenderingApi renderingApi, QWidget* parent = nullptr);

		~A3DViewport() override;
		//A3DViewport();

		void swapBuffers() override;

		glm::uvec2 framebufferSize() const override;
		glm::vec2 framebufferScale() const override;

	protected:

		void initializeGL() override;
		void resizeGL(int w, int h) override;
		void paintGL() override;

	private:

		//std::unique_ptr<a3d::Engine> m_engine;
	};
}

#endif // A3DVIEWPORT_H
