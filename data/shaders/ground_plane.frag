#version 330


in vec3 frag_vertPos;
in vec3 frag_near;
in vec3 frag_far;

uniform mat4 modelMat;
uniform mat4 projMat;

out vec4 fragColor;


float checkerboard(vec2 R, float scale);
float computeDepth(vec3 pos);


void main() {
    float c = (
    int(round(frag_vertPos.x * 5.0)) +
    int(round(frag_vertPos.y * 5.0))
    ) % 2;

    fragColor = vec4(vec3(c/2.0 + 0.3), 1);
}

void main2() {

    float t = -frag_near.y / (frag_far.y- frag_near.y);

    vec3 R = frag_near + t * (frag_far - frag_near);

    float c =
    checkerboard(R.xz, 1) * 0.3 +
    checkerboard(R.xz, 10) * 0.2 +
    checkerboard(R.xz, 100) * 0.1 +
    0.1;
    c = c * float(t > 0);

    float spotlight = min(1.0, 1.5 - 0.02*length(R.xz));

    fragColor = vec4(vec3(c*spotlight), 1);

    // https: //registry.khronos.org/OpenGL-Refpages/gl4/html/gl_FragDepth.xhtml
    gl_FragDepth = computeDepth(R);
}

float checkerboard(vec2 R, float scale) {

    return float((int(floor(R.x / scale)) +
                 int(floor(R.y / scale))
                 ) % 2);
}

// computes Z-buffer depth value, and converts the range.
// ref: https://stackoverflow.com/questions/10264949/glsl-gl-fragcoord-z-calculation-and-setting-gl-fragdepth
float computeDepth(vec3 pos) {

    // get the clip-space coordinates
    vec4 clip_space_pos = projMat * modelMat * vec4(pos.xyz, 1.0);

    // get the depth value in normalized device coordinates
    float clip_space_depth = clip_space_pos.z / clip_space_pos.w;

    // and compute the range based on gl_DepthRange settings (not necessary with default settings, but left for completeness)
    float far = gl_DepthRange.far;
    float near = gl_DepthRange.near;

    float depth = (((far-near) * clip_space_depth) + near + far) / 2.0;

    // and return the result
    return depth;
}
