#version 330

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertex_position_eye;
out vec3 vertex_normal_eye;


void main() {
    vertex_position_eye = vec3(view * model * vec4(vertex_position, 1.0));
    vertex_normal_eye = normalize(vec3(view * model * vec4(vertex_normal, 0.0)));
    gl_Position = projection * vec4(vertex_position_eye, 1.0);
}
