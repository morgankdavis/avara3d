//
//#ifndef AVARA3D_ATTENUATEDLIGHTLEAF_H
//#define AVARA3D_ATTENUATEDLIGHTLEAF_H
//
//
//#include "a3d/rendering/light/AttenuatedLight.h"
//#include "a3d/rendering/light/Light.h"
//
//
//namespace a3d {
//
//	template<class Derived, Light::Kind K>
//	struct AttenuatedLightLeaf : AttenuatedLight {
//		static_assert(K == Light::Kind::Point || K == Light::Kind::Spot,
//					  "AttenuatedLeaf must be Point or Spot");
//	protected:
//		AttenuatedLightLeaf() : AttenuatedLight(K) {}
//	public:
//		static constexpr Light::Kind StaticKind = K;
//		static bool classof(const Light* l) { return l && l->kind() == K; }
//		static bool classof(const Light& l) { return classof(&l); }
//	};
//}
//
//
//#endif //AVARA3D_ATTENUATEDLIGHTLEAF_H
