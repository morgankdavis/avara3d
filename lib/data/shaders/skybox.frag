#header

#include "environment.glsl"
#include "atmospheric_haze.glsl"

#define GAMMA 2.2

in vec3 frag_texCoord;

uniform samplerCube cubeSampler;
uniform mat3 backgroundSampleRotation;

out vec4 fragColor;

void main() {

    vec3 worldDirection = normalize(frag_texCoord);
    vec3 sampleDirection = backgroundSampleRotation * worldDirection;
    fragColor = texture(cubeSampler, sampleDirection);
    fragColor.rgb = ApplyAtmosphericHazeToSky(
            fragColor.rgb,
            worldDirection);

    // gamma correction
    //fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));
}
