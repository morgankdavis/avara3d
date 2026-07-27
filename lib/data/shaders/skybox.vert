<#A3D_SHADER_HEADER#>

layout (location = 0) in vec3 vert_vertPos;

uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 frag_texCoord;

void main() {
    //tex_coord = vertex_position * vec3(-1.0, 1.0, -1.0);
    frag_texCoord = vert_vertPos;
    gl_Position = projMat * viewMat * vec4(vert_vertPos, 1.0);
}
