//
// Created by mkd on 12/8/23.
//

#ifndef AVARA_ENGINE_PHYSICALWORLDMODEL_H
#define AVARA_ENGINE_PHYSICALWORLDMODEL_H


namespace ae {


	class PhysicalWorld;
	class PhysicsBody;


	class PhysicalWorldModel {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicalWorldModel(PhysicalWorld* world);
		~PhysicalWorldModel();

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		virtual void 	add(PhysicsBody& body) = 0;
		virtual void 	remove(PhysicsBody& body) = 0;

		virtual float	gravity() const = 0;
		virtual void	gravity(float gravity) = 0;
	};
}


#endif //AVARA_ENGINE_PHYSICALWORLDMODEL_H
