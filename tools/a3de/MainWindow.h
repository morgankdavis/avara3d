//
//  MainWindow.h
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>

#include "a3d/log/Log.h"

QT_BEGIN_NAMESPACE
namespace Ui {
	class MainWindow;
}
QT_END_NAMESPACE

namespace a3d {
	struct HostUpdateInfo;
	struct RenderFrameInfo;
	class Mesh;
	class Node;
	class Runner;
	class Scene;
	class VisualWorld;
}

namespace a3d::qt {
	class QtViewport;
}

namespace a3de {

	class MainWindow : public QMainWindow {
		Q_OBJECT

		public:

		explicit MainWindow(QWidget* parent = nullptr);
		~MainWindow() override;

	private:

		void initA3D();
		void updateA3D();
		void initLog(a3d::Log::Level level);

		void runnerUpdate(a3d::Runner& runner, const a3d::HostUpdateInfo& info);
		void willRenderCallback(a3d::VisualWorld& world, const a3d::RenderFrameInfo& info);
		void didRenderCallback(a3d::VisualWorld& world, const a3d::RenderFrameInfo& info);

		Ui::MainWindow*					_ui;
		a3d::qt::QtViewport*			_viewport;
		std::unique_ptr<a3d::Scene>		_scene;
		std::unique_ptr<a3d::Runner>	_runner; // Runner must be destroyed before Scene
		std::shared_ptr<a3d::Node>		_pointLightNode;
		std::shared_ptr<a3d::Node>		_bananaNode;
	};
}

#endif // MAINWINDOW_H
