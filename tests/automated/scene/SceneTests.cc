#include <catch2/catch_test_macros.hpp>

#include "a3d/scene/Scene.h"

using namespace a3d;

TEST_CASE("Scene has a root node") {

    Scene scene;

    REQUIRE(scene.rootNode());
}
