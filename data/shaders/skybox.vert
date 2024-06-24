#version 330


layout (location = 0) in vec3 vertPos;

uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 texCoord;


void main() {
    //tex_coord = vertex_position * vec3(-1.0, 1.0, -1.0);
    texCoord = vertPos;
    gl_Position = projMat * viewMat * vec4(vertPos, 1.0);
}
