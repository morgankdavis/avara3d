#version 330

in vec3 tex_coord;

uniform samplerCube cube_texture;

out vec4 frag_colour;


void main () {
    frag_colour = texture(cube_texture, tex_coord);
}
