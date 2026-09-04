#header

//layout(location = 0) in vec3 vert_vertPos;
//layout(location = 1) in vec3 vert_vertColor;
//
//uniform mat4 modelMat;
//uniform mat4 viewMat;
//uniform mat4 projMat;
//
//out vec3 frag_color;
//
//// dashed lines: https://stackoverflow.com/questions/52928678/dashed-line-in-opengl3
//
//void main() {
//	frag_color = vert_vertColor;
//	vec3 vertPos_eye = vec3(viewMat * modelMat * vec4(vert_vertPos, 1.0));
//	gl_Position = projMat * vec4(vertPos_eye, 1.0);
//}

layout(location = 0) in vec3 vert_vertPos;
layout(location = 1) in vec3 vert_vertColor;

out vec3 frag_color;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

void main() {
    frag_color = vert_vertColor;
    gl_Position = projMat * viewMat * modelMat * vec4(vert_vertPos, 1.0);
}
