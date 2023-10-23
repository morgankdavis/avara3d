#version 410


layout (location = 0) in vec3 vertex_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	vec3 vertex_position_eye = vec3(view * model * vec4(vertex_position, 1.0));
	gl_Position = projection * vec4(vertex_position_eye, 1.0);
}
