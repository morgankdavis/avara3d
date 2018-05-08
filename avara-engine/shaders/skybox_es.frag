#version 320 es


precision mediump int;
precision mediump float;


#define GAMMA		2.2


in vec3 tex_coord;

uniform samplerCube cubeSampler;

out vec4 fragColor;


void main () {
    fragColor = texture(cubeSampler, tex_coord);

    // gamma correction
    //fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));
}
