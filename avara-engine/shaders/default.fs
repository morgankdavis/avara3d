#version 330


const float ALPHA_REJECTION_THRESHOLD = 0.5;

const int MATERIAL_MODE_NONE = 0;
const int MATERIAL_MODE_COLOR = 1;
const int MATERIAL_MODE_SAMPLER = 2;


struct Samplers {
	sampler2D ambient;
	sampler2D diffuse;
	sampler2D specular;
};

struct Colors {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Light {
	uint type;
	vec3 position;
	vec3 color;
};


in 			vec3 		vertex_position_eye;
in 			vec3 		vertex_normal_eye;
in 			vec2 		tex_coord;
uniform 	mat4 		view;
uniform 	int 		ambientMode;
uniform 	int 		diffuseMode;
uniform 	int 		specularMode;
uniform		float 		specularExponent;
uniform 	Samplers 	samplers;
uniform 	Colors 		colors;
uniform 	Light 		lights[8];
out 		vec4 		fragColor;


vec3 light_position_world = vec3(70.0, 70.0, 70.0);
vec3 La = vec3(0.2, 0.2, 0.2);
vec3 Ld = vec3(1.0, 1.0, 1.0);
vec3 Ls = vec3(1.0, 1.0, 1.0);


void main () {
	
	// defaults
	vec4 Ka = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 Kd = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 Ks = vec4(0.0, 0.0, 0.0, 1.0);

	switch (ambientMode) {
		case MATERIAL_MODE_COLOR: 	Ka = vec4(colors.ambient, 1.0);						break;
		case MATERIAL_MODE_SAMPLER:	Ka = vec4(texture(samplers.ambient, tex_coord));	break;
	}
	
	switch (diffuseMode) {
		case MATERIAL_MODE_COLOR:	Kd = vec4(colors.diffuse, 1.0);						break;
		case MATERIAL_MODE_SAMPLER:	Kd = vec4(texture(samplers.diffuse, tex_coord));	break;
	}
	
	// look at depth peeling or a-buffers for proper alpha blending
	if (Kd.a < ALPHA_REJECTION_THRESHOLD) discard;
	
	switch (specularMode) {
		case MATERIAL_MODE_COLOR:	Ks = vec4(colors.specular, 1.0);					break;
		case MATERIAL_MODE_SAMPLER:	Ks = vec4(texture(samplers.specular, tex_coord));	break;
	}
	
    // ambient intensity
	
    vec3 Ia = La * vec3(Ka);
    
    // diffuse intensity
	
    // raise light position to eye space
    vec3 light_position_eye = vec3(view * vec4(light_position_world, 1.0));
    vec3 direction_to_light_eye = normalize(light_position_eye - vertex_position_eye);
    float dot_prod_diffuse = max(dot(direction_to_light_eye, vertex_normal_eye), 0.0);
    vec3 Id = Ld * vec3(Kd) * dot_prod_diffuse; // final diffuse intensity

    // specular intensity
	
	vec3 Is = vec3(0.0, 0.0, 0.0);
	if (Ks.x != 0 || Ks.y != 0 || Ks.z != 0) {
		
		vec3 surface_to_viewer_eye = normalize(-vertex_position_eye); // viewer is at 0,0,0
		
//		vec3 reflection_eye = reflect(-direction_to_light_eye, vertex_normal_eye);
//		float dot_prod_specular = dot(reflection_eye, surface_to_viewer_eye);
//		dot_prod_specular = max(dot_prod_specular, 0.0);
//		float specular_factor = pow(dot_prod_specular, specularExponent);
		
		// blinn
		vec3 half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
		float dot_prod_specular = max(dot(half_way_eye, vertex_normal_eye), 0.0);
		float specular_factor = pow(dot_prod_specular, specularExponent); // 200
		
    	Is = Ls * vec3(Ks) * specular_factor; // final specular intensity
	}
	
    fragColor = vec4(Is + Id + Ia, Kd.a);
}
