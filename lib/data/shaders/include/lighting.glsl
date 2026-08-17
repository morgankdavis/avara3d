#ifndef A3D_LIGHTING_GLSL
#define A3D_LIGHTING_GLSL

#include "environment.glsl"

float   Attenuate(float Kc, float Kl, float Kq, float d);
vec3    CalcAmbientLighting(vec3 Ka);
vec3    CalcDirectionalLighting(
	vec3 Kd,
	vec3 Ks,
	vec3 surfacePositionEye,
	vec3 surfaceNormalEye,
	mat4 viewMatrix,
	float surfaceSpecularExponent);
vec3    CalcPointLighting(
	vec3 Kd,
	vec3 Ks,
	vec3 surfacePositionEye,
	vec3 surfaceNormalEye,
	mat4 viewMatrix,
	float surfaceSpecularExponent);
vec3    CalcSpotLighting(
	vec3 Kd,
	vec3 Ks,
	vec3 surfacePositionEye,
	vec3 surfaceNormalEye,
	mat4 viewMatrix,
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
        mat4 viewMatrix, float surfaceSpecularExponent) {

	vec3 color = vec3(0.0);

	for (uint l = 0u; l < Environment.numDirectionalLights; ++l) {

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

vec3 CalcPointLighting(vec3 Kd, vec3 Ks, vec3 surfacePositionEye, vec3 surfaceNormalEye, mat4 viewMatrix,
	float surfaceSpecularExponent) {

	vec3 color = vec3(0.0);

	for (uint l = 0u; l < Environment.numPointLights; ++l) {

		PointLight light = Environment.pointLights[l];

		vec3 L = vec3(light.color.rgb);
		vec3 Id = vec3(0.0);
		vec3 Is = vec3(0.0);

		// diffuse

		vec3 lightPos_eye = vec3(viewMatrix * vec4(light.position_world, 1.0));
		vec3 surfaceToLightDir_eye = normalize(lightPos_eye - surfacePositionEye);
		float dotDiffuse = max(dot(surfaceToLightDir_eye, surfaceNormalEye), 0.0);
		float surfaceToLightDist = distance(lightPos_eye, surfacePositionEye);

		float attenuation = Attenuate(light.constantAttenuation, light.linearAttenuation,
                light.quadraticAttenuation, surfaceToLightDist);

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

vec3 CalcSpotLighting(vec3 Kd, vec3 Ks, vec3 surfacePositionEye, vec3 surfaceNormalEye, mat4 viewMatrix,
	float surfaceSpecularExponent) {

	vec3 color = vec3(0.0);

	for (uint l = 0u; l < Environment.numSpotLights; ++l) {

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

		float linearIntensity = clamp((theta - light.outerAngleCos) / epsilon, 0.0, 1.0);

		if (linearIntensity > 0.0) {

			float easedIntensity = linearIntensity;

			switch (light.featheringMode) {

				case SPOTLIGHT_FEATHERING_MODE_SHARP:
					easedIntensity = clamp(linearIntensity * (2.0 - linearIntensity), 0.0, 1.0);
					break;

				case SPOTLIGHT_FEATHERING_MODE_SOFT:
					easedIntensity = clamp(pow(linearIntensity, 2.0), 0.0, 1.0);
					break;

				default:
					break;
			}

			// diffuse

			float dotDiffuse = max(dot(surfaceToLightDir_eye, surfaceNormalEye), 0.0);
			float surfaceToLightDist = distance(lightPos_eye, surfacePositionEye);

			float attenuation = Attenuate(light.constantAttenuation, light.linearAttenuation,
                    light.quadraticAttenuation, surfaceToLightDist);

			Id = L * Kd * easedIntensity * dotDiffuse * attenuation;

			// specular

			if (Ks.x != 0.0 || Ks.y != 0.0 || Ks.z != 0.0) {

				vec3 reflection_eye = reflect(-surfaceToLightDir_eye, surfaceNormalEye);
				float dotSpecular = dot(reflection_eye, surfaceToCamDir_eye);

				dotSpecular = max(dotSpecular, 0.0);

				float specularFactor = pow(dotSpecular, surfaceSpecularExponent);

				Is = L * Ks * easedIntensity * specularFactor * attenuation;
			}
		}

		color += Id + Is;
	}

	return color;
}

#endif // A3D_LIGHTING_GLSL
