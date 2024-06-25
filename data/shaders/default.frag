#version 410


#define GAMMA 	                    			2.2

#define ALPHA_REJECTION_THRESHOLD 				0.5

#define MATERIAL_PROPERTY_CONTENTS_TYPE_NONE    0
#define MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR	1
#define MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER	2

#define MATERIAL_PROPERTY_TYPE_AMBIENT			0
#define MATERIAL_PROPERTY_TYPE_DIFFUSE			1
#define MATERIAL_PROPERTY_TYPE_SPECULAR			2
#define MATERIAL_PROPERTY_TYPE_EMISSION			3

#define LIGHT_TYPE_AMBIENT 						0
#define LIGHT_TYPE_POINT						1
#define LIGHT_TYPE_DIRECTIONAL 					2
#define LIGHT_TYPE_SPOT 						3


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

layout(std140) struct Light {
	uint 	type;
	float 	PADDING1;
	float 	PADDING2;
	float 	PADDING3;
	vec3 	position_world;
	float 	PADDING4;
	vec3 	color;
	float 	PADDING5;
	float 	attenuationFactor;
//	float 	PADDING6;
//	float 	PADDING7;
//	float 	PADDING8;

//	bool	useDefaultLighting;
//	float 	PADDING9;
//	float 	PADDING10;
//	float 	PADDING11;
	//	float 	attenuationStart;
	//	float 	attenuationEnd;
	//	float 	attenuationExponent;
	//	vec3 	direction_world;
	//	float 	innerAngle;
	//	float 	outerAngle;
};

layout(std140) struct Fog {
	float 	startDistance;
	float 	endDistance;
	float 	densityExponent;
	float 	PADDING1;
	vec4 	color;
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
uniform 	Samplers 	samplers;
uniform 	Colors 		colors;
uniform		bool		useDefaultLighting;
layout(std140) uniform EnvironmentBlock {
	uint	numLights;
	float 	PADDING1;
	float 	PADDING2;
	float 	PADDING3;
	Light 	lights[17]; // MAX_DYNAMIC_LIGHTS + ambient
	Fog 	fog;
};
out 		vec4 		fragColor;


bool FloatsEqual(float a, float b, float eps);
vec4 ColorForTexCoord(vec2 texCoord, uint propertyType, uint propertyContentsType, Colors colors, Samplers samplers);


void main () {

	vec4 Ka = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 Kd = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 Ks = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 Ke = vec4(0.0, 0.0, 0.0, 1.0);
	
	fragColor = vec4(0.0, 0.0, 0.0, 1.0);

	if (useDefaultLighting) {

		// find an emissive property in order:
		// 1. emissive
		// 2. diffuse
		// 3. ambient

		if (emissionContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {
			Ke = ColorForTexCoord(frag_texCoord, MATERIAL_PROPERTY_TYPE_EMISSION, emissionContentsType, colors, samplers);
		}
		else if (diffuseContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {
			Ke = ColorForTexCoord(frag_texCoord, MATERIAL_PROPERTY_TYPE_DIFFUSE, diffuseContentsType, colors, samplers);
		}
		else if (ambientContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {
			Ke = ColorForTexCoord(frag_texCoord, MATERIAL_PROPERTY_TYPE_AMBIENT, ambientContentsType, colors, samplers);
		}
		else {
			Ke = vec4(1.0, 1.0, 1.0, 1.0); // just use white.
		}

		fragColor = vec4(vec3(Ke), 1.0);
	}
	else if (emissionContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE) {
		
		/* emission color */

		Ke = ColorForTexCoord(frag_texCoord, MATERIAL_PROPERTY_TYPE_EMISSION, emissionContentsType, colors, samplers);
		fragColor = vec4(vec3(Ke), 1.0);
		// (no other lighting calculations)
	}
	else {

		/* ambient, diffuse, specular colors */

		Ka = ColorForTexCoord(frag_texCoord, MATERIAL_PROPERTY_TYPE_AMBIENT, ambientContentsType, colors, samplers);
		Kd = ColorForTexCoord(frag_texCoord, MATERIAL_PROPERTY_TYPE_DIFFUSE, diffuseContentsType, colors, samplers);
		Ks = ColorForTexCoord(frag_texCoord, MATERIAL_PROPERTY_TYPE_SPECULAR, specularContentsType, colors, samplers);

		/* lock ambient with diffuse */

		if (locksAmbientWithDiffuse && (diffuseContentsType != MATERIAL_PROPERTY_CONTENTS_TYPE_NONE)) {
			Ka = Kd;
		}

		/* alpha rejection */

		// look at depth peeling or a-buffers for proper alpha blending
		if (Kd.a < ALPHA_REJECTION_THRESHOLD) discard;

		/* lighting */

		for (int l=0; l<numLights; ++l) {
			Light light = lights[l];

			vec3 lightPos_world = light.position_world;
			vec3 La = light.color;
			vec3 Ld = light.color;
			vec3 Ls = light.color;

			vec3 Ia = vec3(0.0, 0.0, 0.0);
			vec3 Id = vec3(0.0, 0.0, 0.0);
			vec3 Is = vec3(0.0, 0.0, 0.0);

			if (light.type == LIGHT_TYPE_AMBIENT) {

				/* ambient */

				Ia = La * vec3(Ka);
				fragColor += vec4(Is + Id + Ia, 0.0);
			}
			else if (light.type == LIGHT_TYPE_POINT) {

				/* point  diffuse */

				// raise light position to eye space
				vec3 lightPos_eye = vec3(viewMat * vec4(lightPos_world, 1.0));
				vec3 directionToLight_eye = normalize(lightPos_eye - frag_vertPos_eye);
				float dotProdDiffuse = max(dot(directionToLight_eye, frag_vertNorm_eye), 0.0);
				
				//Id = Ld * vec3(Kd) * dot_prod_diffuse; // diffuse intensity (original)
				
				float distanceToLight = distance(lightPos_eye, frag_vertPos_eye);
				float attenuation = 1.0 / (1.0 + light.attenuationFactor * pow(distanceToLight, 2.0));
				
				Id = Ld * vec3(Kd) * dotProdDiffuse * attenuation; // diffuse intensity w/attenuation
				
				/* point specular */
				
				Is = vec3(0.0, 0.0, 0.0);
				if (Ks.x != 0.0 || Ks.y != 0.0 || Ks.z != 0.0) {
					
					vec3 surfaceToViewer_eye = normalize(-frag_vertPos_eye); // viewer is at 0,0,0
					
					// phong
					vec3 reflection_eye = reflect(-directionToLight_eye, frag_vertNorm_eye);
					float dotProdSpecular = dot(reflection_eye, surfaceToViewer_eye);
					dotProdSpecular = max(dotProdSpecular, 0.0);
					float specularFactor = pow(dotProdSpecular, specularExponent);
					
					// blinn
//					vec3 half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
//					float dot_prod_specular = max(dot(half_way_eye, vertex_normal_eye), 0.0);
//					float specular_factor = pow(dot_prod_specular, specularExponent);
					
					Is = Ls * vec3(Ks) * specularFactor * attenuation; // specular intensity w/attenuation
				}
				
				fragColor += vec4(Is + Id + Ia, 0.0);
			}
			else if (light.type == LIGHT_TYPE_DIRECTIONAL) {
				
			}
			else if (light.type == LIGHT_TYPE_SPOT) {
				
			}
		}
		
		fragColor = vec4(vec3(fragColor), Kd.a);
	}
	
	
	/* fog */

	if (!FloatsEqual(fog.endDistance, 0.0, 0.0001)) { // endDistance == 0 disables fog
		if (FloatsEqual(fog.densityExponent, 0.0, 0.0001)) { // constant
			fragColor = mix(fragColor, vec4(fog.color.rgb, 1.0), fog.color.a);
		}
		else if (FloatsEqual(fog.densityExponent, 1.0, 0.0001)) { // linear
			float vertDist = length(frag_vertPos_eye);
			float fogFactor = (fog.endDistance - vertDist) / (fog.endDistance - fog.startDistance);
			fogFactor = clamp(fogFactor, 0.0, 1.0);
			fragColor = mix(vec4(fog.color.rgb, 1.0), fragColor, fogFactor);
		}
		else if (fog.densityExponent >= 2.0) { // exponential
			// NOT IMPLEMENTED
			// fogFactor = 1.0-clamp( exp(-fogDensity*fogCoord), 0.0, 1.0)
			// http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=15
		}
	}
	
	
	/* gamma correction */
	
	//fragColor.rgb = pow(fragColor.rgb, vec3(1.0/GAMMA));
}

bool FloatsEqual(float a, float b, float eps) {
	return abs(a-b) <= eps;
}

vec4 ColorForTexCoord(vec2 texCoord, uint propertyType, uint propertyContentsType, Colors colors, Samplers samplers) {

	switch (propertyType) {

		case MATERIAL_PROPERTY_TYPE_AMBIENT:
			switch (propertyContentsType) {
				case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
					return vec4(colors.ambient, 1.0);
				case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
					return vec4(texture(samplers.ambient, texCoord * uvScale));
				default:
					return vec4(0.0, 0.0, 0.0, 1.0);
			}
			break;

		case MATERIAL_PROPERTY_TYPE_DIFFUSE:
			switch (propertyContentsType) {
				case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
					return vec4(colors.diffuse, 1.0);
				case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
					return vec4(texture(samplers.diffuse, texCoord * uvScale));
				default:
					return vec4(0.0, 0.0, 0.0, 1.0);
			}
			break;

		case MATERIAL_PROPERTY_TYPE_SPECULAR:
			switch (propertyContentsType) {
				case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
					return vec4(colors.specular, 1.0);
				case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
					return vec4(texture(samplers.specular, texCoord * uvScale));
				default:
					return vec4(0.0, 0.0, 0.0, 1.0);
			}
			break;

		case MATERIAL_PROPERTY_TYPE_EMISSION:
			switch (propertyContentsType) {
				case MATERIAL_PROPERTY_CONTENTS_TYPE_COLOR:
					return vec4(colors.emission, 1.0);
				case MATERIAL_PROPERTY_CONTENTS_TYPE_SAMPLER:
					return vec4(texture(samplers.emission, texCoord * uvScale));
				default:
					return vec4(0.0, 0.0, 0.0, 1.0);
			}
			break;

		default:
			return vec4(0.0, 0.0, 0.0, 1.0);
	}
}
