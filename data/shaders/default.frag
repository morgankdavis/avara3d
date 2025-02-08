#version 330


// donno if this works
#define FEQ(a, b, eps) (abs(a-b) <= eps)


#define MAX_AMBIENT_LIGHTS								16
#define MAX_DIRECTIONAL_LIGHTS							16
#define MAX_POINT_LIGHTS								128
#define MAX_SPOT_LIGHTS									64


const float GAMMA =										2.2f;

const float ALPHA_REJECTION_THRESHOLD =					0.5f;

const uint MATERIAL_PROPERTY_CONTENTS_TYPE_NONE =		0u;
const uint MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR =		1u;
const uint MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER =	2u;

const uint MATERIAL_PROPERTY_TYPE_AMBIENT =				0u;
const uint MATERIAL_PROPERTY_TYPE_DIFFUSE =				1u;
const uint MATERIAL_PROPERTY_TYPE_SPECULAR =			2u;
const uint MATERIAL_PROPERTY_TYPE_EMISSION =			3u;

const uint LIGHT_TYPE_AMBIENT =							0u;
const uint LIGHT_TYPE_POINT =							1u;
const uint LIGHT_TYPE_DIRECTIONAL =						2u;
const uint LIGHT_TYPE_SPOT =							3u;

const uint SPOTLIGHT_FEATHERING_MODE_LINEAR =			0u;
const uint SPOTLIGHT_FEATHERING_MODE_SHARP =			1u;
const uint SPOTLIGHT_FEATHERING_MODE_SOFT =				2u;


struct Samplers {
	sampler2D ambient;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
};

struct Colors {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
};

struct AmbientLight {
	vec4 	color;
};

struct DirectionalLight {
	vec4 	color;
	vec3 	direction_world;
	float	_PAD0_;
};

struct PointLight {
	vec4 	color;
	vec3 	position_world;
	float	_PAD0_;
	float	constantAttenuation;
	float	linearAttenuation;
	float	quadraticAttenuation;
	float	_PAD1_;
};

struct SpotLight {
	vec4 	color;
	vec3 	position_world;
	float	_PAD0_;
	vec3 	direction_world;
	float	_PAD1_;
	float	innerAngleCos;
	float	outerAngleCos;
	uint	featheringMode;
	float	constantAttenuation;
	float	linearAttenuation;
	float	quadraticAttenuation;
	float	_PAD2_;
	float	_PAD3_;
};

struct Fog {
	vec4 	color;
	float 	startDistance;
	float 	endDistance;
	float 	densityExponent;
	float	_PAD0_;
};

in 			vec3 		frag_vertPos_eye;
in 			vec3 		frag_vertNorm_eye;
in 			vec2 		frag_texCoord;

uniform 	mat4 		viewMat;

// PUT INTO "MaterialBlock" ? {
uniform		bool		useDefaultLighting;
uniform 	uint 		ambientContentsType;
uniform 	uint 		diffuseContentsType;
uniform 	uint 		specularContentsType;
uniform 	uint 		emissionContentsType;
uniform		float 		specularExponent;
uniform		float 		uvScale;
uniform		bool 		locksAmbientWithDiffuse;
uniform 	Samplers 	samplers;
uniform 	Colors 		colors;
// }

layout(std140) uniform EnvironmentBlock {
	uint				numAmbientLights;
	AmbientLight 		ambientLights[MAX_AMBIENT_LIGHTS];
	uint				numDirectionalLights;
	DirectionalLight	directionalLights[MAX_DIRECTIONAL_LIGHTS];
	uint				numPointLights;
	PointLight 			pointLights[MAX_POINT_LIGHTS];
	uint				numSpotLights;
	SpotLight 			spotLights[MAX_SPOT_LIGHTS];
	Fog 				fog;
} Environment;

out 		vec4 		fragColor;


vec4 GetBaseColor(uint propertyType, uint propertyContentsType);
vec4 ApplyDefaultLighting();
vec3 CalcAmbientLighting(vec4 Ka);
vec3 CalcDirectionalLighting(vec4 Kd, vec4 Ks);
vec3 CalcPointLighting(vec4 Kd, vec4 Ks);
vec3 CalcSpotLighting(vec4 Kd, vec4 Ks);
float Attenuate(float Kc, float Kl, float Kq, float d);
vec4 AppleFog(vec4 fragColor);
vec4 ApplyGammaCorrection(vec4 fragColor);
bool FloatsEqual(float a, float b, float eps);


void main () {

	fragColor = vec4(0.0);

	// using default lighting?
	if (useDefaultLighting) {

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

			fragColor.rgb = CalcAmbientLighting(Ka);
			fragColor.rgb += CalcDirectionalLighting(Kd, Ks);
			fragColor.rgb += CalcPointLighting(Kd, Ks);
			fragColor.rgb += CalcSpotLighting(Kd, Ks);

			fragColor = vec4(fragColor.rgb, Kd.a); // pointless?
		}

		// fog

		fragColor += AppleFog(fragColor);
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
					return vec4(texture(samplers.ambient, frag_texCoord * uvScale));
				default:
					return vec4(0.0, 0.0, 0.0, 1.0);
			}
			break;

		case MATERIAL_PROPERTY_TYPE_DIFFUSE:
			switch (propertyContentsType) {
				case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
					return vec4(colors.diffuse, 1.0);
				case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
					return vec4(texture(samplers.diffuse, frag_texCoord * uvScale));
				default:
					return vec4(0.0, 0.0, 0.0, 1.0);
			}
			break;

		case MATERIAL_PROPERTY_TYPE_SPECULAR:
			switch (propertyContentsType) {
				case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
					return vec4(colors.specular, 1.0);
				case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
					return vec4(texture(samplers.specular, frag_texCoord * uvScale));
				default:
					return vec4(0.0, 0.0, 0.0, 1.0);
			}
			break;

		case MATERIAL_PROPERTY_TYPE_EMISSION:
			switch (propertyContentsType) {
				case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
					return vec4(colors.emission, 1.0);
				case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
					return vec4(texture(samplers.emission, frag_texCoord * uvScale));
				default:
					return vec4(0.0, 0.0, 0.0, 1.0);
			}
			break;

		default:
			return vec4(0.0, 0.0, 0.0, 1.0);
	}
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

vec3 CalcAmbientLighting(vec4 Ka) {

	vec3 color = vec3(0.0);

	for (uint l=0u; l<Environment.numAmbientLights; ++l) {

		AmbientLight light = Environment.ambientLights[l];

		vec3 L = vec3(light.color.rgb);

		vec3 Ia = L * vec3(Ka);

		color += Ia;
	}

	return color;
}

vec3 CalcDirectionalLighting(vec4 Kd, vec4 Ks) {

	vec3 color = vec3(0.0);

	for (uint l=0u; l<Environment.numDirectionalLights; ++l) {

		DirectionalLight light = Environment.directionalLights[l];

		vec3 L = vec3(light.color.rgb);

		vec3 Id = vec3(0.0, 0.0, 0.0);
		vec3 Is = vec3(0.0, 0.0, 0.0);

		// diffuse

		vec3 lightDir_eye = vec3(viewMat * vec4(-light.direction_world, 0.0));
		vec3 surfaceToLightDir_eye = normalize(lightDir_eye);
		float dotDiffuse = max(dot(surfaceToLightDir_eye, frag_vertNorm_eye), 0.0);

		Id = L * vec3(Kd) * dotDiffuse;

		// specular

		Is = vec3(0.0, 0.0, 0.0);
		if (Ks.x != 0.0 || Ks.y != 0.0 || Ks.z != 0.0) {

			vec3 surfaceToCamDir = normalize(-frag_vertPos_eye); // viewer is at 0,0,0

			// phong
			vec3 reflection_eye = reflect(-surfaceToLightDir_eye, frag_vertNorm_eye);
			float dotSpecular = dot(reflection_eye, surfaceToCamDir);
			dotSpecular = max(dotSpecular, 0.0);
			float specularFactor = pow(dotSpecular, specularExponent);

			// blinn
			// vec3 half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
			// float dot_prod_specular = max(dot(half_way_eye, vertex_normal_eye), 0.0);
			// float specular_factor = pow(dot_prod_specular, specularExponent);
			
			Is = L * vec3(Ks) * specularFactor;
		}

		color += Id + Is;
	}

	return color;
}

vec3 CalcPointLighting(vec4 Kd, vec4 Ks) {

	vec3 color = vec3(0.0);

	for (uint l=0u; l<Environment.numPointLights; ++l) {

		PointLight light = Environment.pointLights[l];

		vec3 L = vec3(light.color.rgb);

		vec3 Id = vec3(0.0, 0.0, 0.0);
		vec3 Is = vec3(0.0, 0.0, 0.0);

		// diffuse

		// raise light position to eye space
		vec3 lightPos_eye = vec3(viewMat * vec4(light.position_world, 1.0));
		vec3 surfaceToLightDir_eye = normalize(lightPos_eye - frag_vertPos_eye);
		float dotDiffuse = max(dot(surfaceToLightDir_eye, frag_vertNorm_eye), 0.0);

		float surfaceToLightDist = distance(lightPos_eye, frag_vertPos_eye);

		float attenuation = Attenuate(light.constantAttenuation,
									  light.linearAttenuation,
									  light.quadraticAttenuation,
									  surfaceToLightDist);

		Id = L * vec3(Kd) * dotDiffuse * attenuation;

		// specular

		Is = vec3(0.0, 0.0, 0.0);
		if (Ks.x != 0.0 || Ks.y != 0.0 || Ks.z != 0.0) {

			vec3 surfaceToCamDir = normalize(-frag_vertPos_eye); // viewer is at 0,0,0

			// phong
			vec3 reflection_eye = reflect(-surfaceToLightDir_eye, frag_vertNorm_eye);
			float dotSpecular = dot(reflection_eye, surfaceToCamDir);
			dotSpecular = max(dotSpecular, 0.0);
			float specularFactor = pow(dotSpecular, specularExponent);

			// blinn
			// vec3 half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
			// float dot_prod_specular = max(dot(half_way_eye, vertex_normal_eye), 0.0);
			// float specular_factor = pow(dot_prod_specular, specularExponent);

			Is = L * vec3(Ks) * specularFactor * attenuation;
		}

		color += Id + Is;
	}

	return color;
}

vec3 CalcSpotLighting(vec4 Kd, vec4 Ks) {

	vec3 color = vec3(0.0);

	for (uint l=0u; l<Environment.numSpotLights; ++l) {

		SpotLight light = Environment.spotLights[l];

		vec3 L = vec3(light.color.rgb);

		vec3 Id = vec3(0.0, 0.0, 0.0);
		vec3 Is = vec3(0.0, 0.0, 0.0);

		vec3 lightPos_eye = vec3(viewMat * vec4(light.position_world, 1.0));
		vec3 surfaceToLightDir_eye = normalize(lightPos_eye - frag_vertPos_eye);
		vec3 lightDir_eye = normalize(vec3(viewMat * vec4(-light.direction_world, 0.0)));
		vec3 surfaceToCamDir_eye = normalize(-frag_vertPos_eye); // viewer is at 0,0,0

		// cosine of angle. see DeVries 16.5
		float theta = dot(surfaceToLightDir_eye, lightDir_eye);

		float epsilon = light.innerAngleCos - light.outerAngleCos;
		float linearIntensity = clamp((theta - light.outerAngleCos) / epsilon, 0.0, 1.0);

		if (linearIntensity > 0.0) {

			// apply some easing to the light cutoff
			// https://www.geogebra.org/m/kvy5zksn
			// https://learn.pandasuite.com/article/776-animations

			float easedIntensity = linearIntensity;

			switch (light.featheringMode) {
				case SPOTLIGHT_FEATHERING_MODE_SHARP:
					easedIntensity = clamp(linearIntensity * (2-linearIntensity), 0.0, 1.0);
					break;
				case SPOTLIGHT_FEATHERING_MODE_SOFT:
					easedIntensity = clamp(pow(linearIntensity, 2), 0.0, 1.0);
					break;
				default: break;
			}

			// diffuse
			float dotDiffuse = max(dot(surfaceToLightDir_eye, frag_vertNorm_eye), 0.0);
			float surfaceToLightDist = distance(lightPos_eye, frag_vertPos_eye);
			float attenuation = Attenuate(light.constantAttenuation,
										  light.linearAttenuation,
										  light.quadraticAttenuation,
										  surfaceToLightDist);

			Id = L * vec3(Kd) * easedIntensity * dotDiffuse * attenuation;

			// specular

			if (Ks.x != 0.0 || Ks.y != 0.0 || Ks.z != 0.0) {

				// phong
				vec3 reflection_eye = reflect(-surfaceToLightDir_eye, frag_vertNorm_eye);
				float dotSpecular = dot(reflection_eye, surfaceToCamDir_eye);
				dotSpecular = max(dotSpecular, 0.0);
				float specularFactor = pow(dotSpecular, specularExponent);

				Is = L * vec3(Ks) * easedIntensity * specularFactor * attenuation;
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

vec4 AppleFog(vec4 fragColor) {

	vec4 color = vec4(0.0);

	if (!FloatsEqual(Environment.fog.endDistance, 0.0, 0.0001)) { // endDistance == 0 disables fog
		if (FloatsEqual(Environment.fog.densityExponent, 0.0, 0.0001)) { // constant
			fragColor = mix(fragColor, vec4(Environment.fog.color.rgb, 1.0), Environment.fog.color.a);
		}
		else if (FloatsEqual(Environment.fog.densityExponent, 1.0, 0.0001)) { // linear
			float vertDist = length(frag_vertPos_eye);
			float fogFactor = (Environment.fog.endDistance - vertDist)
				/ (Environment.fog.endDistance - Environment.fog.startDistance);
			fogFactor = clamp(fogFactor, 0.0, 1.0);
			color = mix(vec4(Environment.fog.color.rgb, 1.0), fragColor, fogFactor);
		}
		else if (Environment.fog.densityExponent >= 2.0) { // exponential
			// NOT IMPLEMENTED
			// fogFactor = 1.0-clamp( exp(-fogDensity*fogCoord), 0.0, 1.0)
			// http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=15
			color = vec4(1, 0, 0, 1);
		}
	}

	return color;
}

vec4 ApplyGammaCorrection(vec4 fragColor) {

	// ! not implemented !

	//fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));

	return fragColor;
}

bool FloatsEqual(float a, float b, float eps) {
	return abs(a-b) <= eps;
}
