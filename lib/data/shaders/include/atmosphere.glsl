#ifndef A3D_ATMOSPHERE_GLSL
#define A3D_ATMOSPHERE_GLSL

#include "environment.glsl"

const float ATMOSPHERE_EPSILON = 0.00001;
const float ATMOSPHERE_SMALL_HEIGHT_DELTA = 0.001;
const float ATMOSPHERE_MAX_OPTICAL_DEPTH = 50.0;
const float ATMOSPHERE_EXTENT_SCALE_HEIGHTS = 8.0;

// used only for the mathematically infinite horizontal ray of a
// planar atmosphere. The resulting optical depth is clamped later.
const float ATMOSPHERE_MAX_COLUMN_DENSITY = 1000000.0;

// four-point Gauss-Legendre quadrature.
const float ATMOSPHERE_GL_X0 = 0.8611363115940526;
const float ATMOSPHERE_GL_X1 = 0.3399810435848563;
const float ATMOSPHERE_GL_W0 = 0.3478548451374538;
const float ATMOSPHERE_GL_W1 = 0.6521451548625461;

float AtmosphereSurfaceAltitude(
        vec3 worldPosition) {

    if (Environment.surface.type == SURFACE_TYPE_PLANE) {

        return worldPosition.y
        - Environment.surface.planeHeight;
    }

    if (Environment.surface.type == SURFACE_TYPE_SPHERE) {

        return length(
                worldPosition
                - Environment.surface.sphereCenter)
        - Environment.surface.sphereRadius;
    }

    return 0.0;
}

float AtmosphereRelativeDensity(
        vec3 worldPosition) {

    if (Environment.atmosphere.enabled == 0u
        || Environment.surface.type == SURFACE_TYPE_NONE
        || Environment.atmosphere.scaleHeight <= 0.0) {

        return 0.0;
    }

    float altitude =
    AtmosphereSurfaceAltitude(worldPosition);

    float maximumAltitude =
    Environment.atmosphere.scaleHeight
    * ATMOSPHERE_EXTENT_SCALE_HEIGHTS;

    if (altitude < 0.0
        || altitude > maximumAltitude) {

        return 0.0;
    }

    return exp(
            -altitude
            / Environment.atmosphere.scaleHeight);
}

bool AtmosphereRaySphereInterval(
        vec3 rayOrigin,
        vec3 rayDirection,
        vec3 sphereCenter,
        float sphereRadius,
        out float t0,
        out float t1) {

    vec3 originFromCenter =
    rayOrigin - sphereCenter;

    // rayDirection is normalized, so the quadratic's A term is 1.

    float b =
    dot(originFromCenter, rayDirection);

    float c =
    dot(originFromCenter, originFromCenter)
    - sphereRadius * sphereRadius;

    float discriminant =
    b * b - c;

    if (discriminant < 0.0) {
        return false;
    }

    float root =
    sqrt(max(discriminant, 0.0));

    t0 = -b - root;
    t1 = -b + root;

    return true;
}

float AtmosphereIntegrateSphereDensity(
        vec3 rayOrigin,
        vec3 rayDirection,
        float startDistance,
        float endDistance) {

    if (endDistance <= startDistance) {
        return 0.0;
    }

    float midpoint =
    0.5 * (startDistance + endDistance);

    float halfLength =
    0.5 * (endDistance - startDistance);

    float t0 =
    midpoint - halfLength * ATMOSPHERE_GL_X0;

    float t1 =
    midpoint - halfLength * ATMOSPHERE_GL_X1;

    float t2 =
    midpoint + halfLength * ATMOSPHERE_GL_X1;

    float t3 =
    midpoint + halfLength * ATMOSPHERE_GL_X0;

    float d0 =
    AtmosphereRelativeDensity(
            rayOrigin + rayDirection * t0);

    float d1 =
    AtmosphereRelativeDensity(
            rayOrigin + rayDirection * t1);

    float d2 =
    AtmosphereRelativeDensity(
            rayOrigin + rayDirection * t2);

    float d3 =
    AtmosphereRelativeDensity(
            rayOrigin + rayDirection * t3);

    return halfLength
    * (
    ATMOSPHERE_GL_W0 * (d0 + d3)
    + ATMOSPHERE_GL_W1 * (d1 + d2)
    );
}

float AtmosphereSphereColumnDensity(
        vec3 rayOrigin,
        vec3 rayDirection,
        float maximumDistance) {

    vec3 sphereCenter =
    Environment.surface.sphereCenter;

    float surfaceRadius =
    Environment.surface.sphereRadius;

    float atmosphereRadius =
    surfaceRadius
    + Environment.atmosphere.scaleHeight
    * ATMOSPHERE_EXTENT_SCALE_HEIGHTS;

    // a view starting beneath the reference surface does not have a
    // meaningful atmospheric path.

    float originRadius =
    length(rayOrigin - sphereCenter);

    if (originRadius < surfaceRadius - ATMOSPHERE_EPSILON) {
        return 0.0;
    }

    float outerT0;
    float outerT1;

    if (!AtmosphereRaySphereInterval(
            rayOrigin,
            rayDirection,
            sphereCenter,
            atmosphereRadius,
            outerT0,
            outerT1)) {

        return 0.0;
    }

    float startDistance =
    max(0.0, outerT0);

    float endDistance =
    outerT1;

    if (maximumDistance >= 0.0) {

        endDistance =
        min(endDistance, maximumDistance);
    }

    if (endDistance <= startDistance) {
        return 0.0;
    }

    // stop at the physical surface instead of integrating through the
    // sphere and into the atmosphere on the far side.

    float innerT0;
    float innerT1;

    if (AtmosphereRaySphereInterval(
            rayOrigin,
            rayDirection,
            sphereCenter,
            surfaceRadius,
            innerT0,
            innerT1)) {

        if (innerT0 >= startDistance - ATMOSPHERE_EPSILON
            && innerT0 < endDistance) {

            endDistance =
            max(startDistance, innerT0);
        }
    }

    if (endDistance <= startDistance) {
        return 0.0;
    }

    return AtmosphereIntegrateSphereDensity(
            rayOrigin,
            rayDirection,
            startDistance,
            endDistance);
}

float AtmospherePlaneColumnDensity(
        vec3 rayOrigin,
        vec3 rayDirection,
        float maximumDistance) {

    float surfaceHeight =
    Environment.surface.planeHeight;

    float atmosphereHeight =
    surfaceHeight
    + Environment.atmosphere.scaleHeight
    * ATMOSPHERE_EXTENT_SCALE_HEIGHTS;

    if (abs(rayDirection.y) <= ATMOSPHERE_EPSILON) {

        float altitude =
        rayOrigin.y - surfaceHeight;

        float maximumAltitude =
        atmosphereHeight - surfaceHeight;

        if (altitude < 0.0
            || altitude > maximumAltitude) {

            return 0.0;
        }

        float density =
        exp(
                -altitude
                / Environment.atmosphere.scaleHeight);

        if (maximumDistance < 0.0) {

            // an exactly horizontal ray in an infinite planar atmosphere
            // never leaves the layer.

            return ATMOSPHERE_MAX_COLUMN_DENSITY;
        }

        return density * maximumDistance;
    }

    float surfaceDistance =
    (surfaceHeight - rayOrigin.y)
    / rayDirection.y;

    float atmosphereDistance =
    (atmosphereHeight - rayOrigin.y)
    / rayDirection.y;

    float startDistance =
    max(
            0.0,
            min(surfaceDistance, atmosphereDistance));

    float endDistance =
    max(surfaceDistance, atmosphereDistance);

    if (maximumDistance >= 0.0) {

        endDistance =
        min(endDistance, maximumDistance);
    }

    if (endDistance <= startDistance) {
        return 0.0;
    }

    vec3 startPosition =
    rayOrigin + rayDirection * startDistance;

    vec3 endPosition =
    rayOrigin + rayDirection * endDistance;

    float segmentLength =
    endDistance - startDistance;

    float densityAtStart =
    exp(
            -(startPosition.y - surfaceHeight)
            / Environment.atmosphere.scaleHeight);

    float scaledHeightDelta =
    (endPosition.y - startPosition.y)
    / Environment.atmosphere.scaleHeight;

    float integralFactor;

    if (abs(scaledHeightDelta)
        < ATMOSPHERE_SMALL_HEIGHT_DELTA) {

        float x =
        scaledHeightDelta;

        // stable Taylor expansion of:
        //
        // (1 - exp(-x)) / x

        integralFactor =
        1.0
        - 0.5 * x
        + (x * x) / 6.0;
    }
    else {

        integralFactor =
        (
        1.0
        - exp(-scaledHeightDelta)
        )
        / scaledHeightDelta;
    }

    return densityAtStart
    * segmentLength
    * integralFactor;
}

float AtmosphereColumnDensity(
        vec3 rayOrigin,
        vec3 rayDirection,
        float maximumDistance) {

    if (Environment.atmosphere.enabled == 0u
        || Environment.surface.type == SURFACE_TYPE_NONE
        || Environment.atmosphere.scaleHeight <= 0.0) {

        return 0.0;
    }

    float directionLength =
    length(rayDirection);

    if (directionLength <= ATMOSPHERE_EPSILON) {
        return 0.0;
    }

    vec3 direction =
    rayDirection / directionLength;

    if (Environment.surface.type == SURFACE_TYPE_PLANE) {

        return AtmospherePlaneColumnDensity(
                rayOrigin,
                direction,
                maximumDistance);
    }

    if (Environment.surface.type == SURFACE_TYPE_SPHERE) {

        return AtmosphereSphereColumnDensity(
                rayOrigin,
                direction,
                maximumDistance);
    }

    return 0.0;
}

float AtmosphereHazeOpticalDepth(
        vec3 rayStartWorld,
        vec3 rayEndWorld) {

    if (Environment.atmosphere.enabled == 0u
        || Environment.atmosphere.haze.enabled == 0u
        || Environment.atmosphere.haze.density <= 0.0) {

        return 0.0;
    }

    vec3 segment =
    rayEndWorld - rayStartWorld;

    float segmentLength =
    length(segment);

    if (segmentLength <= ATMOSPHERE_EPSILON) {
        return 0.0;
    }

    float columnDensity =
    AtmosphereColumnDensity(
            rayStartWorld,
            segment,
            segmentLength);

    float opticalDepth =
    Environment.atmosphere.haze.density
    * columnDensity;

    return clamp(
            opticalDepth,
            0.0,
            ATMOSPHERE_MAX_OPTICAL_DEPTH);
}

float AtmosphereHazeSkyOpticalDepth(
        vec3 rayStartWorld,
        vec3 rayDirectionWorld) {

    if (Environment.atmosphere.enabled == 0u
        || Environment.atmosphere.haze.enabled == 0u
        || Environment.atmosphere.haze.density <= 0.0) {

        return 0.0;
    }

    float columnDensity =
    AtmosphereColumnDensity(
            rayStartWorld,
            rayDirectionWorld,
            -1.0);

    float opticalDepth =
    Environment.atmosphere.haze.density
    * columnDensity;

    return clamp(
            opticalDepth,
            0.0,
            ATMOSPHERE_MAX_OPTICAL_DEPTH);
}

float AtmosphereHazeBlend(
        float opticalDepth) {

    if (Environment.atmosphere.enabled == 0u
        || Environment.atmosphere.haze.enabled == 0u) {

        return 0.0;
    }

    float transmittance =
    exp(
            -clamp(
                    opticalDepth,
                    0.0,
                    ATMOSPHERE_MAX_OPTICAL_DEPTH));

    float haze =
    1.0 - transmittance;

    return haze
    * clamp(
            Environment.atmosphere.haze.color.a,
            0.0,
            1.0);
}

float AtmosphereLimbGlowAmount(
        vec3 rayOrigin,
        vec3 rayDirection) {

    if (Environment.atmosphere.enabled == 0u
        || Environment.atmosphere.limbGlow.enabled == 0u
        || Environment.atmosphere.limbGlow.intensity <= 0.0
        || Environment.surface.type != SURFACE_TYPE_SPHERE) {

        return 0.0;
    }

    float directionLength =
    length(rayDirection);

    if (directionLength <= ATMOSPHERE_EPSILON) {
        return 0.0;
    }

    vec3 direction =
    rayDirection / directionLength;

    float columnDensity =
    AtmosphereSphereColumnDensity(
            rayOrigin,
            direction,
            -1.0);

    if (columnDensity <= 0.0) {
        return 0.0;
    }

    // A radial outward ray provides a useful baseline atmospheric column:
    //
    //     density(camera) * scaleHeight
    //
    // Limb rays travel through substantially more atmosphere than this.
    // Subtracting the baseline produces a smooth limb enhancement without
    // introducing an artificial angular cutoff at the local horizon.

    float localDensity =
    AtmosphereRelativeDensity(rayOrigin);

    float baselineColumn =
    localDensity
    * Environment.atmosphere.scaleHeight;

    float excessColumn =
    max(
            columnDensity - baselineColumn,
            0.0);

    float normalizedExcess =
    excessColumn
    / Environment.atmosphere.scaleHeight;

    float glow =
    1.0 - exp(-normalizedExcess);

    glow *=
    Environment.atmosphere.limbGlow.intensity;

    glow *=
    clamp(
            Environment.atmosphere.limbGlow.color.a,
            0.0,
            1.0);

    return max(glow, 0.0);
}

vec3 ApplyAtmosphereLimbGlow(
        vec3 color,
        vec3 worldDirection) {

    float glow =
    AtmosphereLimbGlowAmount(
            Environment.viewPosition_world,
            worldDirection);

    return color
    + Environment.atmosphere.limbGlow.color.rgb
    * glow;
}

vec3 ApplyAtmosphereHaze(
        vec3 color,
        vec3 worldPosition) {

    float opticalDepth =
    AtmosphereHazeOpticalDepth(
            Environment.viewPosition_world,
            worldPosition);

    return mix(
            color,
            Environment.atmosphere.haze.color.rgb,
            AtmosphereHazeBlend(opticalDepth));
}

vec3 ApplyAtmosphereToSky(
        vec3 color,
        vec3 worldDirection) {

    float opticalDepth =
    AtmosphereHazeSkyOpticalDepth(
            Environment.viewPosition_world,
            worldDirection);

    vec3 result =
    mix(
            color,
            Environment.atmosphere.haze.color.rgb,
            AtmosphereHazeBlend(opticalDepth));

    result =
    ApplyAtmosphereLimbGlow(
            result,
            worldDirection);

    return result;
}

#endif // A3D_ATMOSPHERE_GLSL
