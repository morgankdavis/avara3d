#ifndef A3D_ATMOSPHERIC_HAZE_GLSL
#define A3D_ATMOSPHERIC_HAZE_GLSL

#include "environment.glsl"

const float ATMOSPHERIC_HAZE_EPSILON = 0.00001;
const float ATMOSPHERIC_HAZE_SMALL_HEIGHT_DELTA = 0.001;
const float ATMOSPHERIC_HAZE_MAX_EXPONENT = 50.0;
const float ATMOSPHERIC_HAZE_MAX_OPTICAL_DEPTH = 50.0;

float AtmosphericHazeDensityAtHeight(float worldHeight);
float AtmosphericHazeOpticalDepth(
        vec3 rayStartWorld,
        vec3 rayEndWorld);
float AtmosphericHazeSkyOpticalDepth(
        vec3 rayStartWorld,
        vec3 rayDirectionWorld);
float AtmosphericHazeBlend(float opticalDepth);
vec3 ApplyAtmosphericHaze(
        vec3 color,
        vec3 worldPosition);
vec3 ApplyAtmosphericHazeToSky(
        vec3 color,
        vec3 worldDirection);

float AtmosphericHazeDensityAtHeight(float worldHeight) {

    if (Environment.atmosphericHaze.enabled == 0u
        || Environment.atmosphericHaze.density <= 0.0) {

        return 0.0;
    }

    float exponent =
    -Environment.atmosphericHaze.heightFalloff
    * (
    worldHeight
    - Environment.atmosphericHaze.baseHeight
    );

    exponent = clamp(
            exponent,
            -ATMOSPHERIC_HAZE_MAX_EXPONENT,
            ATMOSPHERIC_HAZE_MAX_EXPONENT);

    return Environment.atmosphericHaze.density
    * exp(exponent);
}

float AtmosphericHazeOpticalDepth(
        vec3 rayStartWorld,
        vec3 rayEndWorld) {

    if (Environment.atmosphericHaze.enabled == 0u
        || Environment.atmosphericHaze.density <= 0.0) {

        return 0.0;
    }

    vec3 segment =
    rayEndWorld - rayStartWorld;

    float segmentLength =
    length(segment);

    if (segmentLength <= ATMOSPHERIC_HAZE_EPSILON) {
        return 0.0;
    }

    float densityAtStart =
    AtmosphericHazeDensityAtHeight(rayStartWorld.y);

    float scaledHeightDelta =
    Environment.atmosphericHaze.heightFalloff
    * (rayEndWorld.y - rayStartWorld.y);

    float integralFactor;

    if (abs(scaledHeightDelta)
        < ATMOSPHERIC_HAZE_SMALL_HEIGHT_DELTA) {

        // Stable Taylor expansion of:
        //
        // (1 - exp(-x)) / x
        //
        // near x = 0.

        float x = scaledHeightDelta;

        integralFactor =
        1.0
        - 0.5 * x
        + (x * x) / 6.0;
    }
    else if (scaledHeightDelta
        < -ATMOSPHERIC_HAZE_MAX_EXPONENT) {

        // The ray travels far enough downward through exponentially
        // increasing density that it is effectively opaque.

        return ATMOSPHERIC_HAZE_MAX_OPTICAL_DEPTH;
    }
    else {

        integralFactor =
        (
        1.0
        - exp(-scaledHeightDelta)
        )
        / scaledHeightDelta;
    }

    float opticalDepth =
    densityAtStart
    * segmentLength
    * integralFactor;

    return clamp(
            opticalDepth,
            0.0,
            ATMOSPHERIC_HAZE_MAX_OPTICAL_DEPTH);
}

float AtmosphericHazeSkyOpticalDepth(
        vec3 rayStartWorld,
        vec3 rayDirectionWorld) {

    if (Environment.atmosphericHaze.enabled == 0u
        || Environment.atmosphericHaze.density <= 0.0) {

        return 0.0;
    }

    float directionLength =
    length(rayDirectionWorld);

    if (directionLength <= ATMOSPHERIC_HAZE_EPSILON) {
        return 0.0;
    }

    vec3 direction =
    rayDirectionWorld / directionLength;

    // A horizontal or downward infinite ray never escapes the
    // exponentially dense atmospheric layer.

    if (direction.y <= ATMOSPHERIC_HAZE_EPSILON) {
        return ATMOSPHERIC_HAZE_MAX_OPTICAL_DEPTH;
    }

    float densityAtStart =
    AtmosphericHazeDensityAtHeight(rayStartWorld.y);

    float verticalFalloff =
    Environment.atmosphericHaze.heightFalloff
    * direction.y;

    float opticalDepth =
    densityAtStart / verticalFalloff;

    return clamp(
            opticalDepth,
            0.0,
            ATMOSPHERIC_HAZE_MAX_OPTICAL_DEPTH);
}

float AtmosphericHazeBlend(float opticalDepth) {

    if (Environment.atmosphericHaze.enabled == 0u) {
        return 0.0;
    }

    float transmittance =
    exp(
            -clamp(
                    opticalDepth,
                    0.0,
                    ATMOSPHERIC_HAZE_MAX_OPTICAL_DEPTH));

    float haze =
    1.0 - transmittance;

    return haze
    * clamp(
            Environment.atmosphericHaze.color.a,
            0.0,
            1.0);
}

vec3 ApplyAtmosphericHaze(
        vec3 color,
        vec3 worldPosition) {

    float opticalDepth =
    AtmosphericHazeOpticalDepth(
            Environment.viewPosition_world,
            worldPosition);

    return mix(
            color,
            Environment.atmosphericHaze.color.rgb,
            AtmosphericHazeBlend(opticalDepth));
}

vec3 ApplyAtmosphericHazeToSky(
        vec3 color,
        vec3 worldDirection) {

    float opticalDepth =
    AtmosphericHazeSkyOpticalDepth(
            Environment.viewPosition_world,
            worldDirection);

    return mix(
            color,
            Environment.atmosphericHaze.color.rgb,
            AtmosphericHazeBlend(opticalDepth));
}

#endif
