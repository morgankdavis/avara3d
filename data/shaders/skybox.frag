#version 410


#define GAMMA		2.2


in vec3 texCoord;

uniform samplerCube cubeSampler;

out vec4 fragColor;


void main () {
    fragColor = texture(cubeSampler, texCoord);

    // gamma correction
    //fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));
}
