#version 330

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 texture_coordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//uniform vec3 light_position_world;

//uniform uint num_point_lights;
//uniform vec3 point_light_positions[8];
//uniform vec3 point_light_colors[8];

//uniform uint light_position;
//uniform uint light_color;

out vec3 vertex_position_eye;
out vec3 vertex_normal_eye;
out vec2 tex_coord;

//out vec3 light_position_eye;

void main() {
	//vec3 light_position_world = vec3(70.0, 70.0, 70.0);
	//light_position_eye = vec3(view * vec4(light_position_world, 1.0));
	
	
    vertex_position_eye = vec3(view * model * vec4(vertex_position, 1.0));
    vertex_normal_eye = normalize(vec3(view * model * vec4(vertex_normal, 0.0)));
    tex_coord = texture_coordinate;
    gl_Position = projection * vec4(vertex_position_eye, 1.0);
}
