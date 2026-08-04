//
//  App.h
//  005-materialslights
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TEST_MATERIALSLIGHTS_APP_H
#define AVARA3D_TEST_MATERIALSLIGHTS_APP_H

#include <memory>

#include "a3d/Application.h"

namespace a3d {

    class Node;
    class Scene;
    class Window;

}

namespace test::materialslights {

    class App : public a3d::Application {

    public:
        /// Public Lifecycle Functions ///

        App(int argc, char* argv[]);
        ~App() override;

        App(const App&) = delete;
        App& operator=(const App&) = delete;

        App(App&&) = delete;
        App& operator=(App&&) = delete;

    protected:
        /// Protected Member Functions ///

        std::unique_ptr<a3d::Scene> init() override;
        bool                        shouldContinue(const a3d::Scene& scene) override;
        void                        didShutdown() override;

        /// Runner Callbacks ///

        void hostUpdate(a3d::Runner& runner, const a3d::Runner::UpdateInfo& info) override;

    private:
        /// Private Member Variables ///

        std::unique_ptr<a3d::Window> _window;
        a3d::Node*                   _pointLightNode;
    };

}

#endif // AVARA3D_TEST_MATERIALSLIGHTS_APP_H
