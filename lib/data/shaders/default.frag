#header

#include "atmosphere.glsl"
#include "environment.glsl"
#include "fog.glsl"
#include "lighting.glsl"

const float GAMMA = 2.2;

const float ALPHA_REJECTION_THRESHOLD = 0.5;

const uint MATERIAL_PROPERTY_CONTENTS_TYPE_NONE = 0u;
const uint MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR = 1u;
const uint MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER = 2u;

const uint MATERIAL_PROPERTY_TYPE_AMBIENT = 0u;
const uint MATERIAL_PROPERTY_TYPE_DIFFUSE = 1u;
const uint MATERIAL_PROPERTY_TYPE_SPECULAR = 2u;
const uint MATERIAL_PROPERTY_TYPE_EMISSION = 3u;

// GL ES does not like this
//struct Samplers {
//	sampler2D ambient;
//	sampler2D diffuse;
//	sampler2D specular;
//	sampler2D emission;
//};
uniform sampler2D ambientSampler;
uniform sampler2D diffuseSampler;
uniform sampler2D specularSampler;
uniform sampler2D emissionSampler;

struct Colors {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
};

in vec3 frag_vertPos_world;
in vec3 frag_vertPos_eye;
in vec3 frag_vertNorm_eye;
in vec2 frag_texCoord;

uniform     mat4        viewMat;
uniform     uint        ambientContentsType;
uniform     uint        diffuseContentsType;
uniform     uint        specularContentsType;
uniform     uint        emissionContentsType;
uniform     float       specularExponent;
uniform     float       uvScale;
uniform     bool        locksAmbientWithDiffuse;
//uniform 	Samplers 	samplers;
uniform     Colors      colors;
uniform     vec4        tint;

out         vec4        fragColor;

vec4 GetBaseColor(uint propertyType, uint propertyContentsType);
vec4 ApplyDefaultLighting();
vec3 LinearToSRGB(vec3 linear);
vec4 ApplyGammaCorrection(vec4 fragColor);

void main () {

    fragColor = vec4(0.0);

    // using default lighting?
    if (Environment.defaultLightingEnabled > 0u) {

        fragColor = ApplyDefaultLighting();
    }
    else {

        // can skip lighting calcs?
        if (emissionContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {

            fragColor = GetBaseColor(MATERIAL_PROPERTY_TYPE_EMISSION, emissionContentsType);
        }

        // do lighting calcs...
        else {

            // get base colors

            vec4 Ka = GetBaseColor(MATERIAL_PROPERTY_TYPE_AMBIENT, ambientContentsType);
            vec4 Kd = GetBaseColor(MATERIAL_PROPERTY_TYPE_DIFFUSE, diffuseContentsType);
            vec4 Ks = GetBaseColor(MATERIAL_PROPERTY_TYPE_SPECULAR, specularContentsType);

            // lock ambient with diffuse?

            if ((diffuseContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE)) {
                Ka = Kd;
            }

            // alpha rejection

            if (Kd.a < ALPHA_REJECTION_THRESHOLD) discard;

            // dynamic lighting

            vec3 surfaceNormalEye = normalize(frag_vertNorm_eye);

            fragColor.rgb = CalcAmbientLighting(Ka.rgb);

            fragColor.rgb += CalcDirectionalLighting(
                    Kd.rgb,
                    Ks.rgb,
                    frag_vertPos_eye,
                    surfaceNormalEye,
                    viewMat,
                    specularExponent);

            fragColor.rgb += CalcPointLighting(
                    Kd.rgb,
                    Ks.rgb,
                    frag_vertPos_eye,
                    surfaceNormalEye,
                    viewMat,
                    specularExponent);

            fragColor.rgb += CalcSpotLighting(
                    Kd.rgb,
                    Ks.rgb,
                    frag_vertPos_eye,
                    surfaceNormalEye,
                    viewMat,
                    specularExponent);

            fragColor = vec4(fragColor.rgb, Kd.a);
        }
    }

    // per-draw tint

    fragColor.rgb = mix(
            fragColor.rgb,
            tint.rgb,
            clamp(tint.a, 0.0, 1.0)
    );

    // fog

    fragColor = ApplyFog(
            fragColor,
            length(frag_vertPos_eye));

    // atmosphere

    fragColor.rgb = ApplyAtmosphereHaze(
            fragColor.rgb,
            frag_vertPos_world);

    // gamma

    fragColor = ApplyGammaCorrection(fragColor);
}

vec4 GetBaseColor(uint propertyType, uint propertyContentsType) {

    switch (propertyType) {

        case MATERIAL_PROPERTY_TYPE_AMBIENT:
            switch (propertyContentsType) {
                case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
                    return vec4(colors.ambient, 1.0);
                case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
                    return vec4(texture(ambientSampler, frag_texCoord * uvScale));
                default :
                    return vec4(0.0, 0.0, 0.0, 1.0);
            }
            break;

        case MATERIAL_PROPERTY_TYPE_DIFFUSE:
            switch (propertyContentsType) {
                case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
                    return vec4(colors.diffuse, 1.0);
                case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
                    return vec4(texture(diffuseSampler, frag_texCoord * uvScale));
                default :
                    return vec4(0.0, 0.0, 0.0, 1.0);
            }
            break;

        case MATERIAL_PROPERTY_TYPE_SPECULAR:
            switch (propertyContentsType) {
                case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
                    return vec4(colors.specular, 1.0);
                case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
                    return vec4(texture(specularSampler, frag_texCoord * uvScale));
                default :
                    return vec4(0.0, 0.0, 0.0, 1.0);
            }
            break;

        case MATERIAL_PROPERTY_TYPE_EMISSION:
            switch (propertyContentsType) {
                case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
                    return vec4(colors.emission, 1.0);
                case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
                    return vec4(texture(emissionSampler, frag_texCoord * uvScale));
                default :
                    return vec4(0.0, 0.0, 0.0, 1.0);
            }
            break;

        //        default:
        //            return vec4(0.0, 0.0, 0.0, 1.0);
    }

    return vec4(0.0, 0.0, 0.0, 1.0);
}

vec4 ApplyDefaultLighting() {

    // find an emissive property in order of: emissive, diffuse, ambient

    if (emissionContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {
        return GetBaseColor(MATERIAL_PROPERTY_TYPE_EMISSION, emissionContentsType);
    }
    else if (diffuseContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {
        return GetBaseColor(MATERIAL_PROPERTY_TYPE_DIFFUSE, diffuseContentsType);
    }
    else if (ambientContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {
        return GetBaseColor(MATERIAL_PROPERTY_TYPE_AMBIENT, ambientContentsType);
    }

    return vec4(1.0);
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
