#header

#include "environment.glsl"
#include "lighting.glsl"

in vec2 fragNdc;

uniform mat4 viewMat;
uniform mat4 viewProjMat;
uniform mat4 inverseViewProjMat;

uniform float groundHeight;
uniform vec3 groundColor;

uniform bool minorGridEnabled;
uniform vec4 minorGridColor;
uniform float minorGridSpacing;
uniform float minorGridLineWidthPixels;
uniform float minorGridReliefStrength;

uniform bool majorGridEnabled;
uniform vec4 majorGridColor;
uniform float majorGridSpacing;
uniform float majorGridLineWidthPixels;
uniform float majorGridReliefStrength;

uniform bool curvatureEnabled;
uniform vec2 curvatureCenter;
uniform float curvatureRadius;

uniform bool radialFadeEnabled;
uniform vec2 radialFadeCenter;
uniform float radialFadeStartDistance;
uniform float radialFadeEndDistance;
uniform vec4 radialFadeColor;

uniform bool horizonHazeEnabled;
uniform vec4 horizonHazeColor;
uniform float horizonHazeAngularWidthDegrees;

uniform float groundSpecularIntensity;
uniform float groundSpecularExponent;

out vec4 fragColor;

vec3 Unproject(vec2 ndc, float ndcDepth);
float ComputeDepth(vec3 worldPosition);
float GridCoverage(vec2 worldXZ, float spacing, float lineWidthPixels);
vec2 GridReliefGradient(
        vec2 worldXZ,
        float spacing,
        float lineWidthPixels,
        float reliefStrength);
bool IntersectGround(vec3 rayOrigin,
        vec3 ray,
        out vec3 worldPosition,
        out vec3 surfaceNormalWorld);

void main() {

    vec3 nearWorld = Unproject(fragNdc, -1.0);
    vec3 farWorld = Unproject(fragNdc, 1.0);
    vec3 ray = farWorld - nearWorld;

//    if (abs(ray.y) < 0.000001) {
//        discard;
//    }
//
//    float t = (groundHeight - nearWorld.y) / ray.y;
//
//    if (t < 0.0) {
//        discard;
//    }
//
//    vec3 worldPosition = nearWorld + ray * t;
    vec3 worldPosition;
    vec3 surfaceNormalWorld;

    if (!IntersectGround(
            nearWorld,
            ray,
            worldPosition,
            surfaceNormalWorld)) {

        discard;
    }

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

//    vec3 surfacePositionEye = vec3(viewMat * vec4(worldPosition, 1.0));
//    vec3 surfaceNormalEye = normalize(mat3(viewMat) * surfaceNormalWorld);

    vec2 reliefGradient = vec2(0.0);

    if (minorGridEnabled && minorGridReliefStrength != 0.0) {

        reliefGradient += GridReliefGradient(
                worldPosition.xz,
                minorGridSpacing,
                minorGridLineWidthPixels,
                minorGridReliefStrength);
    }

    if (majorGridEnabled && majorGridReliefStrength != 0.0) {

        reliefGradient += GridReliefGradient(
                worldPosition.xz,
                majorGridSpacing,
                majorGridLineWidthPixels,
                majorGridReliefStrength);
    }

    vec3 lightingNormalWorld =
    normalize(
            surfaceNormalWorld
            - vec3(
                    reliefGradient.x,
                    0.0,
                    reliefGradient.y));

    vec3 surfacePositionEye =
    vec3(viewMat * vec4(worldPosition, 1.0));

    vec3 surfaceNormalEye =
    normalize(mat3(viewMat) * lightingNormalWorld);

    vec3 geometricNormalEye =
    normalize(mat3(viewMat) * surfaceNormalWorld);

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

        float blend =
        fade * clamp(radialFadeColor.a, 0.0, 1.0);

        color = mix(
                color,
                radialFadeColor.rgb,
                blend);
    }

    if (horizonHazeEnabled) {

        vec3 eyeToSurfaceDirection =
        normalize(surfacePositionEye);

        float horizonAngle = asin(
                clamp(
                        abs(dot(
                                eyeToSurfaceDirection,
                                geometricNormalEye)),
                        0.0,
                        1.0));

        float haze = 1.0 - smoothstep(
                0.0,
                radians(horizonHazeAngularWidthDegrees),
                horizonAngle);

        haze *= clamp(horizonHazeColor.a, 0.0, 1.0);

        color = mix(
                color,
                horizonHazeColor.rgb,
                haze);
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
    vec2 cellSizePixels = 1.0 / derivative;
    vec2 lodFade = smoothstep(
            vec2(2.0), // 3.0
            vec2(4.0), // 6.0
            cellSizePixels);
    lineCoverage *= lodFade;

    return max(lineCoverage.x, lineCoverage.y);
}

vec2 GridReliefGradient(
        vec2 worldXZ,
        float spacing,
        float lineWidthPixels,
        float reliefStrength) {

    vec2 gridCoord =
    worldXZ / spacing;

    vec2 signedDistanceToLine =
    fract(gridCoord + 0.5) - 0.5;

    vec2 derivative =
    max(fwidth(gridCoord), vec2(0.000001));

    vec2 pixelDistance =
    signedDistanceToLine / derivative;

    float radiusPixels =
    lineWidthPixels * 0.5 + 0.5;

    vec2 normalizedDistance =
    clamp(
            abs(pixelDistance) / radiusPixels,
            vec2(0.0),
            vec2(1.0));

    vec2 profileSlope =
    -4.0
    * normalizedDistance
    * (1.0 - normalizedDistance * normalizedDistance);

    profileSlope *=
    sign(pixelDistance);

    vec2 cellSizePixels =
    1.0 / derivative;

    vec2 lodFade =
    smoothstep(
            vec2(2.0),
            vec2(4.0),
            cellSizePixels);

    return profileSlope
    * lodFade
    * (reliefStrength / radiusPixels);
}

bool IntersectGround(
        vec3 rayOrigin,
        vec3 ray,
        out vec3 worldPosition,
        out vec3 surfaceNormalWorld) {

    if (curvatureEnabled) {

        vec3 sphereCenter =
        vec3(
                curvatureCenter.x,
                groundHeight - curvatureRadius,
                curvatureCenter.y);

        vec3 originFromCenter =
        rayOrigin - sphereCenter;

        float a =
        dot(ray, ray);

        float b =
        2.0 * dot(originFromCenter, ray);

        float c =
        dot(originFromCenter, originFromCenter)
        - curvatureRadius * curvatureRadius;

        float discriminant =
        b * b - 4.0 * a * c;

        if (discriminant < 0.0) {
            return false;
        }

        float sqrtDiscriminant =
        sqrt(max(discriminant, 0.0));

        float inverseTwoA =
        0.5 / a;

        float t0 =
        (-b - sqrtDiscriminant) * inverseTwoA;

        float t1 =
        (-b + sqrtDiscriminant) * inverseTwoA;

        float t;

        if (t0 >= 0.0) {
            t = t0;
        }
        else if (t1 >= 0.0) {
            t = t1;
        }
        else {
            return false;
        }

        worldPosition =
        rayOrigin + ray * t;

        surfaceNormalWorld =
        normalize(worldPosition - sphereCenter);

        return true;
    }

    if (abs(ray.y) < 0.000001) {
        return false;
    }

    float t =
    (groundHeight - rayOrigin.y) / ray.y;

    if (t < 0.0) {
        return false;
    }

    worldPosition =
    rayOrigin + ray * t;

    surfaceNormalWorld =
    vec3(0.0, 1.0, 0.0);

    return true;
}
