#version 330

in vec3 vertex_position_eye;
in vec3 vertex_normal_eye;
in vec2 tex_coord;

in vec3 light_position_eye;

uniform mat4 view;
uniform sampler2D texture_ambient;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;


uniform vec3 light_position_world;
uniform vec3 light_color;



layout (location = 0) out vec4 frag_color;


//vec3 light_position_world = vec3(70.0, 70.0, 70.0);

//vec3 Ls = vec3(1.0, 1.0, 1.0);
//vec3 Ld = vec3(0.7, 0.7, 0.7);
//vec3 La = vec3(0.2, 0.2, 0.2);

//vec3 light_color = vec3(0.5, 0.5, 0.5);


vec3 material_specular = vec3(1.0, 1.0, 1.0);
vec3 material_diffuse = vec3(texture(texture_diffuse, tex_coord));
vec3 material_ambient = vec3(texture(texture_diffuse, tex_coord));


float specular_exponent = 150.0;


void main () {

    // ambient intensity
    vec3 intensity_ambient = light_color * material_ambient;
    
    // diffuse intensity
    vec3 light_position_eye = vec3(view * vec4(light_position_world, 1.0)); // raise light position to eye space
    vec3 direction_to_light_eye = normalize(light_position_eye - vertex_position_eye);
    float dot_prod_diffuse = max(dot(direction_to_light_eye, vertex_normal_eye), 0.0);
    vec3 intensity_diffuse = light_color * material_diffuse * dot_prod_diffuse; // final diffuse intensity

    // specular intensity
    vec3 surface_to_viewer_eye = normalize(-vertex_position_eye); // viewer is at 0,0,0
    
	/*vec3 reflection_eye = reflect(-direction_to_light_eye, vertex_normal_eye);
	float dot_prod_specular = dot(reflection_eye, surface_to_viewer_eye);
	dot_prod_specular = max(dot_prod_specular, 0.0);
	float specular_factor = pow(dot_prod_specular, specular_exponent);*/
	
    // blinn
    vec3 half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
    float dot_prod_specular = max(dot(half_way_eye, vertex_normal_eye), 0.0);
    float specular_factor = pow(dot_prod_specular, specular_exponent); // 200
	
    vec3 intensity_specular = light_color * material_specular * specular_factor; // final specular intensity
    
    // final color
    frag_color = vec4(intensity_ambient + intensity_diffuse + intensity_specular, 1.0);
}
