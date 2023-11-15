//
// Created by mkd on 11/13/23.
//

#ifndef AVARA_ENGINE_PHYSICSSHAPERESOURCES_H
#define AVARA_ENGINE_PHYSICSSHAPERESOURCES_H


#import "Types.h"


namespace ae {


	class PhysicsShape;
	class PhysicsSimulator;


	class PhysicsShapeResources {

		virtual void				get(PhysicsSimulator& simulator,
										std::shared_ptr<PhysicsShape>* shape,
										bool& newlyCreated,
										FrameStats& stats) = 0;
	};
}


#endif //AVARA_ENGINE_PHYSICSSHAPERESOURCES_H

