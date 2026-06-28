#version 330

layout (location = 0) in  vec3 vert_vertPos;
//layout (location = 1) in  vec3 vert_near; // cheating below
//layout (location = 2) in  vec3 vert_far; // cheating below

//uniform mat4 viewMat; // not used yet?
//uniform mat4 projMat; // not used yet?

out vec3 frag_vertPos;
out vec3 frag_near;
out vec3 frag_far;

void main() {

    // CHEATING
    vec3 vert_near = vec3(1, 0.00001, 0);
    vec3 vert_far = vec3(-1, 10000, 0);

//    vec3 vert_near = vec3(0, 0, 0.00001);
//    vec3 vert_far = vec3(0, 0, 10000);

//    vec3 vert_near = vec3(0, 0, 10000);
//    vec3 vert_far = vec3(0, 0, 0.00001);


    frag_vertPos = vert_vertPos;
    frag_near = vert_near;
    frag_far = vert_far;

    gl_Position = vec4(vert_vertPos, 1);
}
