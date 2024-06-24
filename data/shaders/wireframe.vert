#version 330


layout (location = 0) in vec3 vertPos;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

void main() {
	vec3 vertPos_eye = vec3(viewMat * modelMat * vec4(vertPos, 1.0));
	gl_Position = projMat * vec4(vertPos_eye, 1.0);
}
