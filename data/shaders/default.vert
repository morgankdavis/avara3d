#version 330


layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 texture_coordinate;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 vertPos_eye;
out vec3 vertNorm_eye;
out vec2 texCoord;


void main() {
    vertPos_eye = vec3(viewMat * modelMat * vec4(vertex_position, 1.0));
    vertNorm_eye = normalize(vec3(viewMat * modelMat * vec4(vertex_normal, 0.0)));
    texCoord = texture_coordinate;
    gl_Position = projMat * vec4(vertPos_eye, 1.0);
}
