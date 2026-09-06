#header

#include "atmosphere.glsl"
#include "environment.glsl"
#include "fog.glsl"
#include "lighting.glsl"
#include "material.glsl"

const float GAMMA = 2.2;

const float ALPHA_REJECTION_THRESHOLD = 0.5;

in vec3 frag_vertPos_world;
in vec3 frag_vertPos_eye;
in vec3 frag_vertNorm_eye;
in vec2 frag_texCoord;

uniform mat4   viewMat;
uniform vec4   tint;

out vec4 fragColor;

vec3    LinearToSRGB(vec3 linear);
vec4    ApplyGammaCorrection(vec4 fragColor);

void main() {

    fragColor = vec4(0.0);

    // using default lighting?
    if (Environment.defaultLightingEnabled > 0u) {

        fragColor = ApplyDefaultLighting(frag_texCoord);
    }
    else {

        // can skip lighting calcs?
        if (emissionContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {

            fragColor = GetBaseColor(MATERIAL_PROPERTY_TYPE_EMISSION, emissionContentsType, frag_texCoord);
        }

        // do lighting calcs...
        else {

            // get base colors

            vec4 Ka = GetBaseColor(MATERIAL_PROPERTY_TYPE_AMBIENT, ambientContentsType, frag_texCoord);
            vec4 Kd = GetBaseColor(MATERIAL_PROPERTY_TYPE_DIFFUSE, diffuseContentsType, frag_texCoord);
            vec4 Ks = GetBaseColor(MATERIAL_PROPERTY_TYPE_SPECULAR, specularContentsType, frag_texCoord);

            // lock ambient with diffuse?

            if (locksAmbientWithDiffuse && diffuseContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {
                Ka = Kd;
            }

            // alpha rejection

            if (Kd.a < ALPHA_REJECTION_THRESHOLD) {
                discard;
            }

            // dynamic lighting

            vec3 surfaceNormalEye = normalize(frag_vertNorm_eye);

            fragColor.rgb = CalcAmbientLighting(Ka.rgb);

            fragColor.rgb += CalcDirectionalLighting(Kd.rgb, Ks.rgb, frag_vertPos_eye, surfaceNormalEye,
                specularExponent);

            fragColor.rgb += CalcPointLighting(Kd.rgb, Ks.rgb, frag_vertPos_eye, surfaceNormalEye,
                specularExponent);

            fragColor.rgb += CalcSpotLighting(Kd.rgb, Ks.rgb, frag_vertPos_eye, surfaceNormalEye,
                specularExponent);

            fragColor = vec4(fragColor.rgb, Kd.a);
        }
    }

    // per-draw tint

    fragColor.rgb = mix(fragColor.rgb, tint.rgb, clamp(tint.a, 0.0, 1.0));

    // fog

    fragColor = ApplyFog(fragColor, length(frag_vertPos_eye));

    // atmosphere

    fragColor.rgb = ApplyAtmosphereHaze(fragColor.rgb, frag_vertPos_world);

    // gamma

    fragColor = ApplyGammaCorrection(fragColor);
}

vec3 LinearToSRGB(vec3 linear) {
    bvec3 cutoff = lessThanEqual(linear, vec3(0.0031308));
    vec3 lower = linear * 12.92;
    vec3 upper = 1.055 * pow(linear, vec3(1.0 / 2.4)) - 0.055;
    return mix(upper, lower, vec3(cutoff));
}

vec4 ApplyGammaCorrection(vec4 fragColor) {

    // ! UNIMPLEMENTED !
    //return vec4(LinearToSRGB(fragColor.rgb), fragColor.a);
    return fragColor;
}
