#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <vector>

#include <QMainWindow>

#include "a3d/diagnostic/log/Log.h"
//#include "a3d/scene/Node.h"
//#include "a3d/mesh/Mesh.h"


namespace a3d {
//	class Logger;
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




	void initA3D(a3de::A3DViewport& viewport);



private:
	Ui::MainWindow* ui;


	a3de::A3DViewport* _viewport;



	void initLog();
	void logBuildInfo();

	void updateCallback(a3d::Scene& scene, double time, double deltaTime);
	void willRenderCallback(a3d::VisualWorld& world, double time, double deltaTime);
	void didRenderCallback(a3d::VisualWorld& world, double time, double deltaTime);
	void didSimulatePhysicsCallback(a3d::PhysicalWorld& world, double time, double deltaTime);

	std::unique_ptr<a3d::Log>	_log;
	std::shared_ptr<a3d::Node>*		_meshNode;
	std::vector<std::shared_ptr<a3d::Mesh>>*	_meshes;
	std::shared_ptr<a3d::Mesh>*			_mesh;
	a3d::Node*						_pointLightNode;


	std::unique_ptr<a3d::Scene> _scene;

};
#endif // MAINWINDOW_H
