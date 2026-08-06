//
//  PhysicsInventory.h
//  avara3d
//
//  Created by Morgan Davis on 7/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PHYSICSINVENTORY_H
#define AVARA3D_PHYSICS_PHYSICSINVENTORY_H

namespace a3d {

    struct PhysicsInventory {

        unsigned staticBodies {0};
        unsigned dynamicBodies {0};
        unsigned kinematicBodies {0};

        unsigned primitiveShapes {0};
        unsigned boundingBoxShapes {0};
        unsigned convexHullShapes {0};
        unsigned concavePolyhedronShapes {0};
    };

}

#endif //AVARA3D_PHYSICS_PHYSICSINVENTORY_H
