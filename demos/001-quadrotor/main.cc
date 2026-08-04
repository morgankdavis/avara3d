//
//  main.cpp
//  001-quadrotor
//
//  Created by Morgan Davis on 7/19/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include <memory>
#include "a3d/Application.h"
#include "App.h"

int main(int argc, char* argv[]) {
    return a3d::Application::Run(std::make_unique<test::quadrotor::App>(argc, argv));
}
