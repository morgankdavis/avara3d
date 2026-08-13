#ifndef A3D_ENVIRONMENT_GLSL
#define A3D_ENVIRONMENT_GLSL

#define MAX_AMBIENT_LIGHTS 16
#define MAX_DIRECTIONAL_LIGHTS 16
#define MAX_POINT_LIGHTS 128
#define MAX_SPOT_LIGHTS 64

const uint LIGHT_TYPE_AMBIENT = 0u;
const uint LIGHT_TYPE_POINT = 1u;
const uint LIGHT_TYPE_DIRECTIONAL = 2u;
const uint LIGHT_TYPE_SPOT = 3u;

const uint SPOTLIGHT_FEATHERING_MODE_LINEAR = 0u;
const uint SPOTLIGHT_FEATHERING_MODE_SHARP = 1u;
const uint SPOTLIGHT_FEATHERING_MODE_SOFT = 2u;

struct AmbientLight {
    vec4 color;
};

struct DirectionalLight {
    vec4 color;
    vec3 direction_world;
    float _PAD0_;
};

struct PointLight {
    vec4 color;
    vec3 position_world;
    float _PAD0_;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float _PAD1_;
};

struct SpotLight {
    vec4 color;
    vec3 position_world;
    float _PAD0_;
    vec3 direction_world;
    float _PAD1_;
    float innerAngleCos;
    float outerAngleCos;
    uint featheringMode;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float _PAD2_;
    float _PAD3_;
};

struct Fog {
    vec4 color;
    float startDistance;
    float endDistance;
    float transitionExponent;
    uint enabled;
};

struct AtmosphericHaze {
    vec4 color;
    float baseHeight;
    float density;
    float heightFalloff;
    uint enabled;
};

layout(std140) uniform EnvironmentBlock {

    uint defaultLightingEnabled;
    uint _pad0_0;
    uint _pad0_1;
    uint _pad0_2;

    uint numAmbientLights;
    uint _pad1_0;
    uint _pad1_1;
    uint _pad1_2;
    AmbientLight ambientLights[MAX_AMBIENT_LIGHTS];

    uint numDirectionalLights;
    uint _pad2_0;
    uint _pad2_1;
    uint _pad2_2;
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];

    uint numPointLights;
    uint _pad3_0;
    uint _pad3_1;
    uint _pad3_2;
    PointLight pointLights[MAX_POINT_LIGHTS];

    uint numSpotLights;
    uint _pad4_0;
    uint _pad4_1;
    uint _pad4_2;
    SpotLight spotLights[MAX_SPOT_LIGHTS];

    Fog fog;

    vec3 cameraPosition_world;
    float _pad5_0;

    AtmosphericHaze atmosphericHaze;

} Environment;

#endif

