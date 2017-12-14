#version 330

in vec3 vertex_position_eye;
in vec3 vertex_normal_eye;

uniform mat4 view;

layout (location = 0) out vec4 frag_color;



vec3 light_position_world = vec3(70.0, 70.0, 70.0);

vec3 Ld = vec3(0.6, 0.6, 0.6);
vec3 La = vec3(0.4, 0.4, 0.4);

vec3 Kd = vec3(1.0, 1.0, 1.0);
vec3 Ka = vec3(1.0, 1.0, 1.0);



void main () {

    // ambient intensity
	
    vec3 Ia = La * Ka;
    
    // diffuse intensity
	
    // raise light position to eye space
    vec3 light_position_eye = vec3(view * vec4(light_position_world, 1.0));
    vec3 direction_to_light_eye = normalize(light_position_eye - vertex_position_eye);
    float dot_prod_diffuse = max(dot(direction_to_light_eye, vertex_normal_eye), 0.0);
    vec3 Id = Ld * Kd * dot_prod_diffuse; // final diffuse intensity


    // final color
    frag_color = vec4(Id + Ia, 1.0);
}
