#header

in vec2 fragNdc;

uniform mat4 viewProjMat;
uniform mat4 inverseViewProjMat;

uniform float groundHeight;
uniform vec3  groundColor;

out vec4 fragColor;

vec3 Unproject(vec2 ndc, float ndcDepth);
float ComputeDepth(vec3 worldPosition);

void main() {

    vec3 nearWorld = Unproject(fragNdc, -1.0);
    vec3 farWorld = Unproject(fragNdc, 1.0);

    vec3 ray = farWorld - nearWorld;

    if (abs(ray.y) < 0.000001) {
        discard;
    }

    float t = (groundHeight - nearWorld.y) / ray.y;

    if (t < 0.0) {
        discard;
    }

    vec3 worldPosition = nearWorld + ray * t;

    fragColor = vec4(groundColor, 1.0);

    gl_FragDepth = ComputeDepth(worldPosition);
}

vec3 Unproject(vec2 ndc, float ndcDepth) {

    vec4 world = inverseViewProjMat * vec4(ndc, ndcDepth, 1.0);

    return world.xyz / world.w;
}

float ComputeDepth(vec3 worldPosition) {

    vec4 clip = viewProjMat * vec4(worldPosition, 1.0);

    float ndcDepth = clip.z / clip.w;
    float depth = ndcDepth * 0.5 + 0.5;

    return clamp(depth, 0.0, 1.0);
}
