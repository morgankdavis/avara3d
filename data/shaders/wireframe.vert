#version 300 es
//#version 330

layout (location = 0) in vec3 vert_vertPos;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

void main() {
	vec3 vertPos_eye = vec3(viewMat * modelMat * vec4(vert_vertPos, 1.0));
	gl_Position = projMat * vec4(vertPos_eye, 1.0);
}
