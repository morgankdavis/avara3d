//
//  main.cpp
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {

    QApplication     a(argc, argv);
    a3de::MainWindow w;
    w.show();
    return a.exec();
}
