#version 330


layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNorm;
layout (location = 2) in vec2 vert_texCoord;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 vertPos_eye;
out vec3 vertNorm_eye;
out vec2 frag_texCoord;


void main() {
    vertPos_eye = vec3(viewMat * modelMat * vec4(vertPos, 1.0));
    vertNorm_eye = normalize(vec3(viewMat * modelMat * vec4(vertNorm, 0.0)));
    frag_texCoord = vert_texCoord;
    gl_Position = projMat * vec4(vertPos_eye, 1.0);
}
