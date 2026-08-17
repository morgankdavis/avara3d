#ifndef A3D_FOG_GLSL
#define A3D_FOG_GLSL

#include "environment.glsl"

const float FOG_EPSILON = 0.0001;

vec4 ApplyFog(vec4 color, float distance) {

	if (Environment.fog.enabled == 0u) {
		return color;
	}

	// transitionExponent == 0:
	// constant fog intensity using fog color alpha.

	if (abs(Environment.fog.transitionExponent) <= FOG_EPSILON) {
		return mix(color, vec4(Environment.fog.color.rgb, color.a), Environment.fog.color.a);
	}

	float fogAmount =
		(distance - Environment.fog.startDistance) / (Environment.fog.endDistance - Environment.fog.startDistance);

	fogAmount = clamp(fogAmount, 0.0, 1.0);

	// 1 = linear, 2 = quadratic, etc.

	fogAmount = pow(fogAmount, Environment.fog.transitionExponent);

	return mix(color, vec4(Environment.fog.color.rgb, color.a), fogAmount);
}

#endif // A3D_FOG_GLSL
