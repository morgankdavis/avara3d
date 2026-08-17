#header

#include "atmosphere.glsl"
#include "environment.glsl"

#define GAMMA 2.2

in vec3 frag_texCoord;

uniform samplerCube cubeSampler;
uniform mat3        backgroundSampleRotation;
uniform bool        backgroundUsesCubemap;
uniform vec4        backgroundColor;

out vec4 fragColor;

void main() {

	vec3 worldDirection = normalize(frag_texCoord);

	if (backgroundUsesCubemap) {

		vec3 sampleDirection = backgroundSampleRotation * worldDirection;
		fragColor = texture(cubeSampler, sampleDirection);
	}
	else {

		fragColor = backgroundColor;
	}

	fragColor.rgb = ApplyAtmosphereToSky(fragColor.rgb, worldDirection);

	// gamma correction
	//fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));
}
