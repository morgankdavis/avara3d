#include <catch2/catch_test_macros.hpp>

#include "a3d/Runner.h"
#include "a3d/scene/Scene.h"

using namespace a3d;

TEST_CASE("Runner is initially idle") {

    Scene  scene;
    Runner runner(scene);

    REQUIRE(runner.state() == Runner::State::Idle);
}
