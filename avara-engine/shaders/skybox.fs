#version 330

in vec3 tex_coord;

uniform samplerCube cubeSampler;

out vec4 fragColor;


void main () {
    fragColor = texture(cubeSampler, tex_coord);
}
