#ifndef A3D_LIGHTING_GLSL
#define A3D_LIGHTING_GLSL

#include "environment.glsl"

float   Attenuate(float Kc, float Kl, float Kq, float d);
vec3    CalcAmbientLighting(vec3 Ka);
vec3    CalcDirectionalLighting(vec3 Kd, vec3 Ks, vec3 surfacePositionEye, vec3 surfaceNormalEye,
                                float surfaceSpecularExponent);
vec3    CalcPointLighting(vec3 Kd, vec3 Ks, vec3 surfacePositionEye, vec3 surfaceNormalEye,
                          float surfaceSpecularExponent);
vec3    CalcSpotLighting(vec3 Kd, vec3 Ks, vec3 surfacePositionEye, vec3 surfaceNormalEye,
                         float surfaceSpecularExponent);

float Attenuate(float Kc, float Kl, float Kq, float d) {

    float attenuation = 1.0 / (Kc + (Kl * d) + (Kq * d * d));
    return clamp(attenuation, 0.0, 1.0);
}

vec3 CalcAmbientLighting(vec3 Ka) {

    vec3 color = vec3(0.0);

    for (uint l = 0u; l < Environment.numAmbientLights; ++l) {

        AmbientLight light = Environment.ambientLights[l];

        vec3 L = vec3(light.color.rgb);
        vec3 Ia = L * Ka;

        color += Ia;
    }

    return color;
}

vec3 CalcDirectionalLighting(vec3 Kd, vec3 Ks, vec3 surfacePositionEye, vec3 surfaceNormalEye,
                             float surfaceSpecularExponent) {

    vec3 color = vec3(0.0);

    bool hasSpecular = any(notEqual(Ks, vec3(0.0)));
    vec3 surfaceToCamDir_eye =
    hasSpecular ? normalize(-surfacePositionEye) : vec3(0.0);

    for (uint l = 0u; l < Environment.numDirectionalLights; ++l) {

        DirectionalLight light = Environment.directionalLights[l];

        vec3 L = light.color.rgb * light.intensity;
        vec3 Id = vec3(0.0);
        vec3 Is = vec3(0.0);

        // direction_eye is the direction the light points,
        // therefore the surface-to-light direction is its inverse.
        vec3 surfaceToLightDir_eye = -light.direction_eye;

        float dotDiffuse =
        max(dot(surfaceToLightDir_eye, surfaceNormalEye), 0.0);

        Id = L * Kd * dotDiffuse;

        if (hasSpecular) {

            vec3 reflection_eye =
            reflect(-surfaceToLightDir_eye, surfaceNormalEye);

            float dotSpecular =
            max(dot(reflection_eye, surfaceToCamDir_eye), 0.0);

            float specularFactor =
            pow(dotSpecular, surfaceSpecularExponent);

            Is = L * Ks * specularFactor;
        }

        color += Id + Is;
    }

    return color;
}

vec3 CalcPointLighting(vec3 Kd, vec3 Ks, vec3 surfacePositionEye, vec3 surfaceNormalEye,
                       float surfaceSpecularExponent) {

    vec3 color = vec3(0.0);

    bool hasSpecular = any(notEqual(Ks, vec3(0.0)));
    vec3 surfaceToCamDir_eye =
    hasSpecular ? normalize(-surfacePositionEye) : vec3(0.0);

    for (uint l = 0u; l < Environment.numPointLights; ++l) {

        PointLight light = Environment.pointLights[l];

        vec3 L = light.color.rgb * light.intensity;
        vec3 Id = vec3(0.0);
        vec3 Is = vec3(0.0);

        vec3 surfaceToLight_eye =
        light.position_eye - surfacePositionEye;

        float surfaceToLightDist =
        length(surfaceToLight_eye);

        vec3 surfaceToLightDir_eye =
        surfaceToLight_eye / max(surfaceToLightDist, 0.000001);

        float dotDiffuse =
        max(dot(surfaceToLightDir_eye, surfaceNormalEye), 0.0);

        float attenuation =
        Attenuate(
            light.constantAttenuation,
            light.linearAttenuation,
            light.quadraticAttenuation,
            surfaceToLightDist);

        Id = L * Kd * dotDiffuse * attenuation;

        if (hasSpecular) {

            vec3 reflection_eye =
            reflect(-surfaceToLightDir_eye, surfaceNormalEye);

            float dotSpecular =
            max(dot(reflection_eye, surfaceToCamDir_eye), 0.0);

            float specularFactor =
            pow(dotSpecular, surfaceSpecularExponent);

            Is = L * Ks * specularFactor * attenuation;
        }

        color += Id + Is;
    }

    return color;
}

vec3 CalcSpotLighting(vec3 Kd, vec3 Ks, vec3 surfacePositionEye, vec3 surfaceNormalEye,
                      float surfaceSpecularExponent) {

    vec3 color = vec3(0.0);

    bool hasSpecular = any(notEqual(Ks, vec3(0.0)));
    vec3 surfaceToCamDir_eye =
    hasSpecular ? normalize(-surfacePositionEye) : vec3(0.0);

    for (uint l = 0u; l < Environment.numSpotLights; ++l) {

        SpotLight light = Environment.spotLights[l];

        vec3 L = light.color.rgb * light.intensity;
        vec3 Id = vec3(0.0);
        vec3 Is = vec3(0.0);

        vec3 surfaceToLight_eye =
        light.position_eye - surfacePositionEye;

        float surfaceToLightDist =
        length(surfaceToLight_eye);

        vec3 surfaceToLightDir_eye =
        surfaceToLight_eye / max(surfaceToLightDist, 0.000001);

        float theta =
        dot(surfaceToLightDir_eye, -light.direction_eye);

        float epsilon =
        light.innerAngleCos - light.outerAngleCos;

        float linearSpotFactor =
        clamp(
            (theta - light.outerAngleCos) / epsilon,
            0.0,
            1.0);

        if (linearSpotFactor > 0.0) {

            float spotFactor = linearSpotFactor;

            switch (light.featheringMode) {

                case SPOTLIGHT_FEATHERING_MODE_SHARP:
                    spotFactor =
                    clamp(
                        linearSpotFactor * (2.0 - linearSpotFactor),
                        0.0,
                        1.0);
                    break;

                case SPOTLIGHT_FEATHERING_MODE_SOFT:
                    spotFactor =
                    clamp(pow(linearSpotFactor, 2.0), 0.0, 1.0);
                    break;

                default:
                    break;
            }

            float dotDiffuse =
            max(dot(surfaceToLightDir_eye, surfaceNormalEye), 0.0);

            float attenuation =
            Attenuate(
                light.constantAttenuation,
                light.linearAttenuation,
                light.quadraticAttenuation,
                surfaceToLightDist);

            Id =
            L * Kd * spotFactor * dotDiffuse * attenuation;

            if (hasSpecular) {

                vec3 reflection_eye =
                reflect(-surfaceToLightDir_eye, surfaceNormalEye);

                float dotSpecular =
                max(dot(reflection_eye, surfaceToCamDir_eye), 0.0);

                float specularFactor =
                pow(dotSpecular, surfaceSpecularExponent);

                Is =
                L * Ks * spotFactor * specularFactor * attenuation;
            }
        }

        color += Id + Is;
    }

    return color;
}

#endif // A3D_LIGHTING_GLSL
