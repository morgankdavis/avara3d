#header

in 	vec3 frag_color;
out	vec4 fragColor;

// dashed
//flat in vec3 startPos;
//in vec3 vertPos;
//const vec2  u_resolution = vec2(1280, 768);
//const float u_dashSize = 50;
//const float u_gapSize = 50;

void main () {
	fragColor = vec4(frag_color, 1.0);

	// dashed
//	vec2  dir  = (vertPos.xy-startPos.xy) * u_resolution/2.0;
//	float dist = length(dir);
//
//	if (fract(dist / (u_dashSize + u_gapSize)) > u_dashSize/(u_dashSize + u_gapSize)) discard;
//	fragColor = vec4(color, 1.0);
}
