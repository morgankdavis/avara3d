#version 300 es
//#version 330

#ifdef GL_ES
precision highp float;
precision highp int;
#endif

layout(location = 0) out vec4 fragColor;

void main() {
	fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
