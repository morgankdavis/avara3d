#header

#include "environment.glsl"
#include "lighting.glsl"

in vec2 fragNdc;

uniform mat4 viewMat;
uniform mat4 viewProjMat;
uniform mat4 inverseViewProjMat;

uniform float groundHeight;
uniform vec3 groundColor;
uniform float groundSpecularIntensity;
uniform float groundSpecularExponent;

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

    vec3 surfacePositionEye = vec3(viewMat * vec4(worldPosition, 1.0));
    vec3 surfaceNormalEye = normalize(mat3(viewMat) * vec3(0.0, 1.0, 0.0));

    vec3 color;

    if (Environment.useDefaultLighting > 0u) {

        color = groundColor;
    }
    else {

        vec3 Ka = groundColor;
        vec3 Kd = groundColor;
        vec3 Ks = vec3(groundSpecularIntensity);

        color = CalcAmbientLighting(Ka);

        color += CalcDirectionalLighting(
                Kd,
                Ks,
                surfacePositionEye,
                surfaceNormalEye,
                viewMat,
                groundSpecularExponent);

        color += CalcPointLighting(
                Kd,
                Ks,
                surfacePositionEye,
                surfaceNormalEye,
                viewMat,
                groundSpecularExponent);

        color += CalcSpotLighting(
                Kd,
                Ks,
                surfacePositionEye,
                surfaceNormalEye,
                viewMat,
                groundSpecularExponent);
    }

    fragColor = vec4(color, 1.0);

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
