#version 330


layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 color;

void main() {
	color = vertex_color;
	vec3 vertex_position_eye = vec3(view * vec4(vertex_position, 1.0));
	//vec3 vertex_position_eye = vec3(view * model * vec4(vertex_position, 1.0));
	gl_Position = projection * vec4(vertex_position_eye, 1.0);
}
