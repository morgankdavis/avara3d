//
//#ifndef AVARA3D_LIGHTLEAF_H
//#define AVARA3D_LIGHTLEAF_H
//
//
//#include "a3d/rendering/light/Light.h"
//
//
//namespace a3d {
//
//	template<class Derived, Light::Kind K>
//	class LightLeaf : Light {
//
//	protected:
//
//		LightLeaf() : Light(K) {}
//
//	public:
//
//		static constexpr Light::Kind StaticKind = K;
//		static bool classof(const Light *l) { return l && l->kind() == K; }
//		static bool classof(const Light &l) { return classof(&l); }
//	};
//}
//
//
//#endif //AVARA3D_LIGHTLEAF_H
