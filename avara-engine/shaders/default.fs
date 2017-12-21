#version 330


#define ALPHA_REJECTION_THRESHOLD 	0.5

#define MATERIAL_MODE_NONE 			0
#define MATERIAL_MODE_COLOR			1
#define MATERIAL_MODE_SAMPLER 		2

#define LIGHT_TYPE_AMBIENT 			0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_DIRECTIONAL 		2
#define LIGHT_TYPE_SPOT 			3


struct Samplers {
	sampler2D ambient;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emissive;
};

struct Colors {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emissive;
};

struct Light {
	int 	type;
	float 	PADDING1;
	float 	PADDING2;
	float 	PADDING3;
	vec3 	position_world;
	float 	PADDING4;
	vec3 	color;
	float 	PADDING5;
//	vec3 	direction_world;
//	float 	attenuationStart;
//	float 	attenuationEnd;
//	float 	attenuationExponent;
//	float 	innerAngle;
//	float 	outerAngle;
};


in 			vec3 		vertex_position_eye;
in 			vec3 		vertex_normal_eye;
in 			vec2 		tex_coord;
uniform 	mat4 		view;
uniform 	int 		ambientMode;
uniform 	int 		diffuseMode;
uniform 	int 		specularMode;
uniform 	int 		emissiveMode;
uniform		float 		specularExponent;
uniform 	Samplers 	samplers;
uniform 	Colors 		colors;
layout(std140) uniform LightBlock {
	int		numLights;
	float 	PADDING1;
	float 	PADDING2;
	float 	PADDING3;
	Light 	lights[8];
};
out 		vec4 		fragColor;


void main () {

	vec4 Ka = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 Kd = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 Ks = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 Ke = vec4(0.0, 0.0, 0.0, 1.0);
	
	switch (emissiveMode) {
		case MATERIAL_MODE_COLOR:	Ke = vec4(colors.emissive, 1.0);					break;
		case MATERIAL_MODE_SAMPLER:	Ke = vec4(texture(samplers.emissive, tex_coord));	break;
	}
	
	if (emissiveMode != MATERIAL_MODE_NONE) { // nothing else mattress
		fragColor += vec4(vec3(Ke), 1.0);
	}
	else {
		switch (ambientMode) {
			case MATERIAL_MODE_COLOR: 	Ka = vec4(colors.ambient, 1.0);						break;
			case MATERIAL_MODE_SAMPLER:	Ka = vec4(texture(samplers.ambient, tex_coord));	break;
		}
		
		switch (diffuseMode) {
			case MATERIAL_MODE_COLOR:	Kd = vec4(colors.diffuse, 1.0);						break;
			case MATERIAL_MODE_SAMPLER:	Kd = vec4(texture(samplers.diffuse, tex_coord));	break;
		}
		
		switch (specularMode) {
			case MATERIAL_MODE_COLOR:	Ks = vec4(colors.specular, 1.0);					break;
			case MATERIAL_MODE_SAMPLER:	Ks = vec4(texture(samplers.specular, tex_coord));	break;
		}
		
		// look at depth peeling or a-buffers for proper alpha blending
		if (Kd.a < ALPHA_REJECTION_THRESHOLD) discard;
		
		for (int l=0; l<numLights; ++l) {
			Light light = lights[l];
			
			vec3 light_position_world = light.position_world;
			vec3 La = light.color;
			vec3 Ld = light.color;
			vec3 Ls = light.color;
			
			vec3 Ia = vec3(0.0, 0.0, 0.0);
			vec3 Id = vec3(0.0, 0.0, 0.0);
			vec3 Is = vec3(0.0, 0.0, 0.0);
			
			if (light.type == LIGHT_TYPE_AMBIENT) {
				
				// ambient intensity
				
				Ia = La * vec3(Ka);
				fragColor += vec4(Is + Id + Ia, 0.0);
			}
			else if (light.type == LIGHT_TYPE_POINT) {
				
				// diffuse intensity
				
				// raise light position to eye space
				vec3 light_position_eye = vec3(view * vec4(light_position_world, 1.0));
				vec3 direction_to_light_eye = normalize(light_position_eye - vertex_position_eye);
				float dot_prod_diffuse = max(dot(direction_to_light_eye, vertex_normal_eye), 0.0);
				Id = Ld * vec3(Kd) * dot_prod_diffuse; // final diffuse intensity
				
				// specular intensity
				
				Is = vec3(0.0, 0.0, 0.0);
				if (Ks.x != 0 || Ks.y != 0 || Ks.z != 0) {
					
					vec3 surface_to_viewer_eye = normalize(-vertex_position_eye); // viewer is at 0,0,0
					
					// phong
					//				vec3 reflection_eye = reflect(-direction_to_light_eye, vertex_normal_eye);
					//				float dot_prod_specular = dot(reflection_eye, surface_to_viewer_eye);
					//				dot_prod_specular = max(dot_prod_specular, 0.0);
					//				float specular_factor = pow(dot_prod_specular, specularExponent);
					
					// blinn
					vec3 half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
					float dot_prod_specular = max(dot(half_way_eye, vertex_normal_eye), 0.0);
					float specular_factor = pow(dot_prod_specular, specularExponent); // 200
					
					Is = Ls * vec3(Ks) * specular_factor; // final specular intensity
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
}
