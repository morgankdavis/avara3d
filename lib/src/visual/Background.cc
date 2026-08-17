//
//  Background.cc
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/Background.h"

#include <utility>

using namespace a3d;
using namespace std;

Background::Background(Material::Property contents):
    contents {std::move(contents)} {}
