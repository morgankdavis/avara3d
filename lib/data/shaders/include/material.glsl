#ifndef A3D_MATERIAL_GLSL
#define A3D_MATERIAL_GLSL

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

uniform uint  ambientContentsType;
uniform uint  diffuseContentsType;
uniform uint  specularContentsType;
uniform uint  emissionContentsType;
uniform float specularExponent;
uniform float uvScale;
uniform bool  locksAmbientWithDiffuse;
uniform Colors colors;

vec4 GetBaseColor(uint propertyType, uint propertyContentsType, vec2 texCoord) {

    switch (propertyType) {

        case MATERIAL_PROPERTY_TYPE_AMBIENT:
            switch (propertyContentsType) {
                case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
                    return vec4(colors.ambient, 1.0);
                case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
                    return texture(ambientSampler, texCoord / uvScale);
                default :
                    return vec4(0.0, 0.0, 0.0, 1.0);
            }

        case MATERIAL_PROPERTY_TYPE_DIFFUSE:
            switch (propertyContentsType) {
                case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
                    return vec4(colors.diffuse, 1.0);
                case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
                    return texture(diffuseSampler, texCoord / uvScale);
                default :
                    return vec4(0.0, 0.0, 0.0, 1.0);
            }

        case MATERIAL_PROPERTY_TYPE_SPECULAR:
            switch (propertyContentsType) {
                case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
                    return vec4(colors.specular, 1.0);
                case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
                    return texture(specularSampler, texCoord / uvScale);
                default :
                    return vec4(0.0, 0.0, 0.0, 1.0);
            }

        case MATERIAL_PROPERTY_TYPE_EMISSION:
            switch (propertyContentsType) {
                case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
                    return vec4(colors.emission, 1.0);
                case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
                    return texture(emissionSampler, texCoord / uvScale);
                default :
                    return vec4(0.0, 0.0, 0.0, 1.0);
            }
    }

    return vec4(0.0, 0.0, 0.0, 1.0);
}

vec4 ApplyDefaultLighting(vec2 texCoord) {

    if (emissionContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {
        return GetBaseColor(MATERIAL_PROPERTY_TYPE_EMISSION, emissionContentsType, texCoord);
    }
    else if (diffuseContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {
        return GetBaseColor(MATERIAL_PROPERTY_TYPE_DIFFUSE, diffuseContentsType, texCoord);
    }
    else if (ambientContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {
        return GetBaseColor(MATERIAL_PROPERTY_TYPE_AMBIENT, ambientContentsType, texCoord);
    }

    return vec4(1.0);
}

#endif // A3D_MATERIAL_GLSL
