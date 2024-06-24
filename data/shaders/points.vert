// UNUSED


#version 330


layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertColor;

//uniform mat4 model;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 color;

void main() {
	color = vertColor;
	vec3 vertPos_eye = vec3(viewMat * vec4(vertPos, 1.0));
	//vec3 vertex_position_eye = vec3(view * model * vec4(vertex_position, 1.0));
	gl_Position = projMat * vec4(vertPos_eye, 1.0);
}
