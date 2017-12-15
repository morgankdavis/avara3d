#version 330


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
//uniform		float 		specularExponent;
uniform 	Samplers 	samplers;
uniform 	Colors 		colors;
uniform 	Light 		lights[8];
out 		vec4 		frag_color;


float specularExponent = 150.0;


vec3 light_position_world = vec3(70.0, 70.0, 70.0);
vec3 La = vec3(0.2, 0.2, 0.2);
vec3 Ld = vec3(1.0, 1.0, 1.0);
vec3 Ls = vec3(1.0, 1.0, 1.0);


void main () {
	
	// defaults
	vec3 Ka = vec3(0.75, 0.75, 0.75);
	vec3 Kd = vec3(1.0, 1.0, 1.0);
	vec3 Ks = vec3(0.0, 0.0, 0.0);

	switch (ambientMode) {
		case MATERIAL_MODE_COLOR: 	Ka = colors.ambient; 								break;
		case MATERIAL_MODE_SAMPLER:	Ka = vec3(texture(samplers.ambient, tex_coord));	break;
	}
	
	switch (diffuseMode) {
		case MATERIAL_MODE_COLOR:	Kd = colors.diffuse;								break;
		case MATERIAL_MODE_SAMPLER:	Kd = vec3(texture(samplers.diffuse, tex_coord));	break;
	}
	
	switch (specularMode) {
		case MATERIAL_MODE_COLOR:	Ks = colors.specular;								break;
		case MATERIAL_MODE_SAMPLER:	Ks = vec3(texture(samplers.specular, tex_coord));	break;
	}
	
    // ambient intensity
	
    vec3 Ia = La * Ka;
    
    // diffuse intensity
	
    // raise light position to eye space
    vec3 light_position_eye = vec3(view * vec4(light_position_world, 1.0));
    vec3 direction_to_light_eye = normalize(light_position_eye - vertex_position_eye);
    float dot_prod_diffuse = max(dot(direction_to_light_eye, vertex_normal_eye), 0.0);
    vec3 Id = Ld * Kd * dot_prod_diffuse; // final diffuse intensity

    // specular intensity
	
    vec3 surface_to_viewer_eye = normalize(-vertex_position_eye); // viewer is at 0,0,0
    
//    vec3 reflection_eye = reflect(-direction_to_light_eye, vertex_normal_eye);
//    float dot_prod_specular = dot(reflection_eye, surface_to_viewer_eye);
//    dot_prod_specular = max(dot_prod_specular, 0.0);
//    float specular_factor = pow(dot_prod_specular, specular_exponent);
    
    // blinn
    vec3 half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
    float dot_prod_specular = max(dot(half_way_eye, vertex_normal_eye), 0.0);
    float specular_factor = pow(dot_prod_specular, specularExponent); // 200
    
    vec3 Is = Ls * Ks * specular_factor; // final specular intensity
	
    frag_color = vec4(Is + Id + Ia, 1.0);
}
