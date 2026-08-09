#header

#include "environment.glsl"
#include "lighting.glsl"

in vec2 fragNdc;

uniform mat4 viewMat;
uniform mat4 viewProjMat;
uniform mat4 inverseViewProjMat;

uniform float groundHeight;
uniform vec3 groundColor;

uniform bool radialFadeEnabled;
uniform vec2 radialFadeCenter;
uniform float radialFadeStartDistance;
uniform float radialFadeEndDistance;

uniform bool minorGridEnabled;
uniform vec4 minorGridColor;
uniform float minorGridSpacing;
uniform float minorGridLineWidthPixels;

uniform bool majorGridEnabled;
uniform vec4 majorGridColor;
uniform float majorGridSpacing;
uniform float majorGridLineWidthPixels;

uniform vec3 radialFadeColor;

uniform float groundSpecularIntensity;
uniform float groundSpecularExponent;

out vec4 fragColor;

vec3 Unproject(vec2 ndc, float ndcDepth);
float ComputeDepth(vec3 worldPosition);
float GridCoverage(vec2 worldXZ, float spacing, float lineWidthPixels);

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

    vec3 surfaceColor = groundColor;

    if (minorGridEnabled) {

        float coverage = GridCoverage(
                worldPosition.xz,
                minorGridSpacing,
                minorGridLineWidthPixels);

        float blend = coverage * clamp(minorGridColor.a, 0.0, 1.0);

        surfaceColor = mix(surfaceColor, minorGridColor.rgb, blend);
    }

    if (majorGridEnabled) {

        float coverage = GridCoverage(
                worldPosition.xz,
                majorGridSpacing,
                majorGridLineWidthPixels);

        float blend = coverage * clamp(majorGridColor.a, 0.0, 1.0);

        surfaceColor = mix(surfaceColor, majorGridColor.rgb, blend);
    }

    vec3 surfacePositionEye = vec3(viewMat * vec4(worldPosition, 1.0));

    vec3 surfaceNormalEye = normalize(mat3(viewMat) * vec3(0.0, 1.0, 0.0));

    vec3 color;

    if (Environment.useDefaultLighting > 0u) {

        color = surfaceColor;
    }
    else {

        vec3 Ka = surfaceColor;
        vec3 Kd = surfaceColor;
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

    if (radialFadeEnabled) {

        float radialDistance = length(worldPosition.xz - radialFadeCenter);

        float fade = smoothstep(
                radialFadeStartDistance,
                radialFadeEndDistance,
                radialDistance);

        color = mix(color, radialFadeColor, fade);
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

float GridCoverage(
        vec2 worldXZ,
        float spacing,
        float lineWidthPixels) {

    vec2 gridCoord = worldXZ / spacing;

    vec2 distanceToLine = abs(fract(gridCoord - 0.5) - 0.5);

    vec2 derivative = max(fwidth(gridCoord), vec2(0.000001));

    vec2 pixelDistance = distanceToLine / derivative;

    float halfWidth = lineWidthPixels * 0.5;

    vec2 lineCoverage = 1.0 - smoothstep(
            vec2(halfWidth - 0.5),
            vec2(halfWidth + 0.5),
            pixelDistance);

    return max(lineCoverage.x, lineCoverage.y);
}
