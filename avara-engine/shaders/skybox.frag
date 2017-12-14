#version 330

in vec3 tex_coord;

uniform samplerCube cubeSampler;

out vec4 frag_colour;


void main () {
    frag_colour = texture(cubeSampler, tex_coord);
}
