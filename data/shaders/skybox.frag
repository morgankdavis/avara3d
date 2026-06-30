#version 300 es
//#version 330

#ifdef GL_ES
precision highp float;
precision highp int;
#endif

#define GAMMA		2.2

in vec3 frag_texCoord;

uniform samplerCube cubeSampler;

out vec4 fragColor;

void main () {
    fragColor = texture(cubeSampler, frag_texCoord);

    // gamma correction
    //fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));
}
