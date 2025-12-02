#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <vector>

#include <QMainWindow>

#include "a3d/diagnostic/log/Log.h"


namespace a3d {
	class Mesh;
	class Node;
	class Scene;
	class PhysicalWorld;
	class VisualWorld;
}


namespace a3de {
	class A3DViewport;
}


QT_BEGIN_NAMESPACE
namespace Ui {
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:

	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private:

	void initScene(a3de::A3DViewport &viewport);
	void initLog();
	void logBuildInfo();

	void updateCallback(a3d::Scene& scene, double time, double deltaTime);
	void willRenderCallback(a3d::VisualWorld& world, double time, double deltaTime);
	void didRenderCallback(a3d::VisualWorld& world, double time, double deltaTime);
	void didSimulatePhysicsCallback(a3d::PhysicalWorld& world, double time, double deltaTime);

	Ui::MainWindow*				_ui;
	a3de::A3DViewport*			_viewport;
	std::unique_ptr<a3d::Log>	_log;
	std::unique_ptr<a3d::Scene> _scene;
	a3d::Node*					_pointLightNode;
};

#endif // MAINWINDOW_H
