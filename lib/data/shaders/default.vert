#header

layout (location = 0) in vec3 vert_vertPos;
layout (location = 1) in vec3 vert_vertNorm;
layout (location = 2) in vec2 vert_texCoord;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat3 normalMat;

out vec3 frag_vertPos_world;
out vec3 frag_vertPos_eye;
out vec3 frag_vertNorm_eye;
out vec2 frag_texCoord;

void main() {

    vec4 vertPosWorld = modelMat * vec4(vert_vertPos, 1.0);
    frag_vertPos_world = vertPosWorld.xyz;
    frag_vertPos_eye = vec3(viewMat * vertPosWorld);
    frag_vertNorm_eye = normalMat * vert_vertNorm; // for normals under non-uniform scale
    frag_texCoord = vert_texCoord;
    gl_Position = projMat * vec4(frag_vertPos_eye, 1.0);
}
