#header

#include "environment.glsl"

const float GAMMA =										2.2;

const float ALPHA_REJECTION_THRESHOLD =					0.5;

const uint MATERIAL_PROPERTY_CONTENTS_TYPE_NONE =		0u;
const uint MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR =		1u;
const uint MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER =	2u;

const uint MATERIAL_PROPERTY_TYPE_AMBIENT =				0u;
const uint MATERIAL_PROPERTY_TYPE_DIFFUSE =				1u;
const uint MATERIAL_PROPERTY_TYPE_SPECULAR =			2u;
const uint MATERIAL_PROPERTY_TYPE_EMISSION =			3u;

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

in 			vec3 		frag_vertPos_eye;
in 			vec3 		frag_vertNorm_eye;
in 			vec2 		frag_texCoord;

uniform 	mat4 		viewMat;
uniform 	uint 		ambientContentsType;
uniform 	uint 		diffuseContentsType;
uniform 	uint 		specularContentsType;
uniform 	uint 		emissionContentsType;
uniform		float 		specularExponent;
uniform		float 		uvScale;
uniform		bool 		locksAmbientWithDiffuse;
//uniform 	Samplers 	samplers;
uniform 	Colors 		colors;

out 		vec4 		fragColor;

vec4 GetBaseColor(uint propertyType, uint propertyContentsType);
vec4 ApplyDefaultLighting();

vec3 CalcAmbientLighting(vec3 Ka);

vec3 CalcDirectionalLighting(
        vec3 Kd,
        vec3 Ks,
        vec3 surfacePositionEye,
        vec3 surfaceNormalEye,
        mat4 viewMatrix,
        float surfaceSpecularExponent);

vec3 CalcPointLighting(
        vec3 Kd,
        vec3 Ks,
        vec3 surfacePositionEye,
        vec3 surfaceNormalEye,
        mat4 viewMatrix,
        float surfaceSpecularExponent);

vec3 CalcSpotLighting(
        vec3 Kd,
        vec3 Ks,
        vec3 surfacePositionEye,
        vec3 surfaceNormalEye,
        mat4 viewMatrix,
        float surfaceSpecularExponent);

float Attenuate(float Kc, float Kl, float Kq, float d);
vec4 ApplyFog(vec4 fragColor);
vec3 LinearToSRGB(vec3 linear);
vec4 ApplyGammaCorrection(vec4 fragColor);
bool FloatsEqual(float a, float b, float eps);

void main () {

    fragColor = vec4(0.0);

    // using default lighting?
    if (Environment.useDefaultLighting > 0u) {

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

            fragColor.rgb = CalcAmbientLighting(Ka.rgb);

            fragColor.rgb += CalcDirectionalLighting(Kd.rgb,
                    Ks.rgb,
                    frag_vertPos_eye,
                    frag_vertNorm_eye,
                    viewMat,
                    specularExponent);

            fragColor.rgb += CalcPointLighting(
                    Kd.rgb,
                    Ks.rgb,
                    frag_vertPos_eye,
                    frag_vertNorm_eye,
                    viewMat,
                    specularExponent);

            fragColor.rgb += CalcSpotLighting(
                    Kd.rgb,
                    Ks.rgb,
                    frag_vertPos_eye,
                    frag_vertNorm_eye,
                    viewMat,
                    specularExponent);

            fragColor = vec4(fragColor.rgb, Kd.a); // pointless?
        }

        // fog

        fragColor = ApplyFog(fragColor);
    }

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
                default:
                    return vec4(0.0, 0.0, 0.0, 1.0);
            }
            break;

        case MATERIAL_PROPERTY_TYPE_DIFFUSE:
            switch (propertyContentsType) {
                case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
                    return vec4(colors.diffuse, 1.0);
                case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
                    return vec4(texture(diffuseSampler, frag_texCoord * uvScale));
                default:
                    return vec4(0.0, 0.0, 0.0, 1.0);
            }
            break;

        case MATERIAL_PROPERTY_TYPE_SPECULAR:
            switch (propertyContentsType) {
                case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
                    return vec4(colors.specular, 1.0);
                case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
                    return vec4(texture(specularSampler, frag_texCoord * uvScale));
                default:
                    return vec4(0.0, 0.0, 0.0, 1.0);
            }
            break;

        case MATERIAL_PROPERTY_TYPE_EMISSION:
            switch (propertyContentsType) {
                case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
                    return vec4(colors.emission, 1.0);
                case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
                    return vec4(texture(emissionSampler, frag_texCoord * uvScale));
                default:
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

vec3 CalcAmbientLighting(vec3 Ka) {

    vec3 color = vec3(0.0);

    for (uint l=0u; l<Environment.numAmbientLights; ++l) {
        AmbientLight light = Environment.ambientLights[l];
        vec3 L = vec3(light.color.rgb);
        vec3 Ia = L * Ka;
        color += Ia;
    }

    return color;
}

vec3 CalcDirectionalLighting(
        vec3 Kd,
        vec3 Ks,
        vec3 surfacePositionEye,
        vec3 surfaceNormalEye,
        mat4 viewMatrix,
        float surfaceSpecularExponent) {

    vec3 color = vec3(0.0);

    for (uint l=0u; l<Environment.numDirectionalLights; ++l) {

        DirectionalLight light = Environment.directionalLights[l];

        vec3 L = vec3(light.color.rgb);
        vec3 Id = vec3(0.0);
        vec3 Is = vec3(0.0);

        // diffuse

        vec3 lightDir_eye = vec3(viewMatrix * vec4(-light.direction_world, 0.0));
        vec3 surfaceToLightDir_eye = normalize(lightDir_eye);
        float dotDiffuse = max(dot(surfaceToLightDir_eye, surfaceNormalEye), 0.0);

        Id = L * Kd * dotDiffuse;

        // specular

        if (Ks.x != 0.0 || Ks.y != 0.0 || Ks.z != 0.0) {

            vec3 surfaceToCamDir = normalize(-surfacePositionEye); // viewer is at 0,0,0

            // phong

            vec3 reflection_eye = reflect(-surfaceToLightDir_eye, surfaceNormalEye);
            float dotSpecular = dot(reflection_eye, surfaceToCamDir);
            dotSpecular = max(dotSpecular, 0.0);
            float specularFactor = pow(dotSpecular, surfaceSpecularExponent);

            // blinn

            // vec3 halfWay_eye = normalize(surfaceToCamDir + surfaceToLightDir_eye);
            // float dotSpecular = max(dot(halfWay_eye, surfaceNormalEye), 0.0);
            // float specularFactor = pow(dotSpecular, surfaceSpecularExponent);

            Is = L * Ks * specularFactor;
        }

        color += Id + Is;
    }

    return color;
}

vec3 CalcPointLighting(
        vec3 Kd,
        vec3 Ks,
        vec3 surfacePositionEye,
        vec3 surfaceNormalEye,
        mat4 viewMatrix,
        float surfaceSpecularExponent) {

    vec3 color = vec3(0.0);

    for (uint l=0u; l<Environment.numPointLights; ++l) {

        PointLight light = Environment.pointLights[l];

        vec3 L = vec3(light.color.rgb);

        vec3 Id = vec3(0.0);
        vec3 Is = vec3(0.0);

        // diffuse

        vec3 lightPos_eye = vec3(viewMatrix * vec4(light.position_world, 1.0));
        vec3 surfaceToLightDir_eye = normalize(lightPos_eye - surfacePositionEye);
        float dotDiffuse = max(dot(surfaceToLightDir_eye, surfaceNormalEye), 0.0);
        float surfaceToLightDist = distance(lightPos_eye, surfacePositionEye);
        float attenuation = Attenuate(
                light.constantAttenuation,
                light.linearAttenuation,
                light.quadraticAttenuation,
                surfaceToLightDist);

        Id = L * Kd * dotDiffuse * attenuation;

        // specular

        if (Ks.x != 0.0 || Ks.y != 0.0 || Ks.z != 0.0) {

            vec3 surfaceToCamDir = normalize(-surfacePositionEye);
            vec3 reflection_eye = reflect(-surfaceToLightDir_eye, surfaceNormalEye);
            float dotSpecular = dot(reflection_eye, surfaceToCamDir);
            dotSpecular = max(dotSpecular, 0.0);
            float specularFactor = pow(dotSpecular, surfaceSpecularExponent);

            Is = L * Ks * specularFactor * attenuation;
        }

        color += Id + Is;
    }

    return color;
}

vec3 CalcSpotLighting(
        vec3 Kd,
        vec3 Ks,
        vec3 surfacePositionEye,
        vec3 surfaceNormalEye,
        mat4 viewMatrix,
        float surfaceSpecularExponent) {

    vec3 color = vec3(0.0);

    for (uint l=0u; l<Environment.numSpotLights; ++l) {

        SpotLight light = Environment.spotLights[l];

        vec3 L = vec3(light.color.rgb);
        vec3 Id = vec3(0.0);
        vec3 Is = vec3(0.0);

        vec3 lightPos_eye = vec3(viewMatrix * vec4(light.position_world, 1.0));
        vec3 surfaceToLightDir_eye = normalize(lightPos_eye - surfacePositionEye);
        vec3 lightDir_eye = normalize(vec3(viewMatrix * vec4(-light.direction_world, 0.0)));
        vec3 surfaceToCamDir_eye = normalize(-surfacePositionEye);

        // cosine of angle. see DeVries 16.5

        float theta = dot(surfaceToLightDir_eye, lightDir_eye);
        float epsilon = light.innerAngleCos - light.outerAngleCos;
        float linearIntensity = clamp(
                (theta - light.outerAngleCos) / epsilon,
                0.0,
                1.0);

        if (linearIntensity > 0.0) {

            float easedIntensity = linearIntensity;

            switch (light.featheringMode) {

                case SPOTLIGHT_FEATHERING_MODE_SHARP:
                    easedIntensity = clamp(
                            linearIntensity * (2.0 - linearIntensity),
                            0.0,
                            1.0);
                    break;

                case SPOTLIGHT_FEATHERING_MODE_SOFT:
                    easedIntensity = clamp(
                            pow(linearIntensity, 2.0),
                            0.0,
                            1.0);
                    break;

                default:
                    break;
            }

            // diffuse

            float dotDiffuse = max(
                    dot(surfaceToLightDir_eye, surfaceNormalEye),
                    0.0);

            float surfaceToLightDist = distance(lightPos_eye, surfacePositionEye);

            float attenuation = Attenuate(
                    light.constantAttenuation,
                    light.linearAttenuation,
                    light.quadraticAttenuation,
                    surfaceToLightDist);

            Id = L * Kd
            * easedIntensity
            * dotDiffuse
            * attenuation;

            // specular

            if (Ks.x != 0.0 || Ks.y != 0.0 || Ks.z != 0.0) {

                vec3 reflection_eye = reflect(
                        -surfaceToLightDir_eye,
                        surfaceNormalEye);

                float dotSpecular = dot(
                        reflection_eye,
                        surfaceToCamDir_eye);

                dotSpecular = max(dotSpecular, 0.0);

                float specularFactor = pow(
                        dotSpecular,
                        surfaceSpecularExponent);

                Is =
                L * Ks
                * easedIntensity
                * specularFactor
                * attenuation;
            }
        }

        color += Id + Is;
    }

    return color;
}

float Attenuate(float Kc, float Kl, float Kq, float d) {

    // A = 1 / 1.0 + (Kc + (Kl * d) + (Kq * d^2))

    //	const float EPS = .0000001;

    float attenuation = 1.0;

    //	if (!FloatsEqual(Kc, 0.0, EPS)
    //	|| !FloatsEqual(Kl, 0.0, EPS)
    //	|| !FloatsEqual(Kq, 0.0, EPS)) {
    attenuation = 1.0 / (Kc + (Kl * d) + (Kq * d*d));
    //	}

    return clamp(attenuation, 0.0, 1.0);
}

vec4 ApplyFog(vec4 fragColor) {

    if (Environment.fog.enabled == 0u) {
        return fragColor;
    }

    // transitionExponent == 0:
    // constant fog intensity, using fog color alpha

    if (FloatsEqual(Environment.fog.transitionExponent, 0.0, 0.0001)) {
        return mix(
                fragColor,
                vec4(Environment.fog.color.rgb, fragColor.a),
                Environment.fog.color.a);
    }

    float fragmentDistance = length(frag_vertPos_eye);

    float fogAmount = (
    fragmentDistance - Environment.fog.startDistance
    ) / (
    Environment.fog.endDistance -
    Environment.fog.startDistance
    );

    fogAmount = clamp(fogAmount, 0.0, 1.0);

    // 1 = linear, 2 = quadratic, etc.

    fogAmount = pow(
            fogAmount,
            Environment.fog.transitionExponent);

    return mix(
            fragColor,
            vec4(Environment.fog.color.rgb, fragColor.a),
            fogAmount);
}

vec3 LinearToSRGB(vec3 linear) {
    bvec3 cutoff = lessThanEqual(linear, vec3(0.0031308));
    vec3 lower = linear * 12.92;
    vec3 upper = 1.055 * pow(linear, vec3(1.0/2.4)) - 0.055;
    return mix(upper, lower, vec3(cutoff));
}

vec4 ApplyGammaCorrection(vec4 fragColor) {

    //return vec4(LinearToSRGB(fragColor.rgb), fragColor.a);
    return fragColor;
}

bool FloatsEqual(float a, float b, float eps) {
    return abs(a-b) <= eps;
}
