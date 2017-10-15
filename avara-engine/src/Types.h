//
//  Types.h
//	avara-engine
//
//  Created by Morgan Davis on 10/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Types_h
#define Types_h


namespace ae {

	typedef enum {
		AntialiasingModeNone =	0,
		AntialiasingMode2X =	2,
		AntialiasingMode4X =	4,
		AntialiasingMode8X =	8,
		AntialiasingMode16X =	16
	} AntialiasingMode;

	typedef enum {
		SceneLoadingOptionValidateStructure,
		SceneLoadingOptionPreTransform,
		SceneLoadingOptionImproveCacheLocality
	} SceneLoadingOption;

	typedef enum {
		ProgramTypeDefault,
		ProgramTypePhongPlain, // temporary
		ProgramTypePhongTexture, // temporary
		ProgramTypeFancy // temporary
	} ProgramType;

	typedef enum {
		WrapModeClamp,
		WrapModeRepeat,
		WrapModeClampToBorder,
		WrapModeMirror
	} WrapMode;


	typedef enum {
		WrapModeNone,
		WrapModeNearest,
		WrapModeLinear
	} FilterMode;


	typedef enum {
		LightTypeAmbient,
		LightTypeOmni,
		LightTypeDirectional,
		LightTypeSpot
	} LightType;
}


#endif /* Types_h */
