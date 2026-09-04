#header

uniform vec4 tint;

layout(location = 0) out vec4 fragColor;

void main() {

    fragColor = vec4(1.0);

    // per-draw tint
    fragColor.rgb = mix(fragColor.rgb, tint.rgb, clamp(tint.a, 0.0, 1.0));
}
