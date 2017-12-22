#version 330


in vec3 vertex_position;

uniform mat4 view;
uniform mat4 projection;

out vec3 tex_coord;


void main() {
    tex_coord = vertex_position;
    gl_Position = projection * view * vec4(vertex_position, 1.0);
}
