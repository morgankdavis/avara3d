#ifndef A3D_ENVIRONMENT_GLSL
#define A3D_ENVIRONMENT_GLSL

#define MAX_AMBIENT_LIGHTS     16
#define MAX_DIRECTIONAL_LIGHTS 16
#define MAX_POINT_LIGHTS       128
#define MAX_SPOT_LIGHTS        64

const uint LIGHT_TYPE_AMBIENT = 0u;
const uint LIGHT_TYPE_POINT = 1u;
const uint LIGHT_TYPE_DIRECTIONAL = 2u;
const uint LIGHT_TYPE_SPOT = 3u;

const uint SPOTLIGHT_FEATHERING_MODE_LINEAR = 0u;
const uint SPOTLIGHT_FEATHERING_MODE_SHARP = 1u;
const uint SPOTLIGHT_FEATHERING_MODE_SOFT = 2u;

const uint SURFACE_TYPE_NONE = 0u;
const uint SURFACE_TYPE_PLANE = 1u;
const uint SURFACE_TYPE_SPHERE = 2u;

struct AmbientLight {
    vec4 color;
};

struct DirectionalLight {
    vec4  color;
    vec3  direction_world;
    float intensity;
};

struct PointLight {
    vec4  color;
    vec3  position_world;
    float intensity;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float _PAD0_;
};

struct SpotLight {
    vec4  color;
    vec3  position_world;
    float intensity;
    vec3  direction_world;
    float _PAD0_;
    float innerAngleCos;
    float outerAngleCos;
    uint  featheringMode;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float _PAD1_;
    float _PAD2_;
};

struct Surface {
    uint  type;
    float planeHeight;
    float _PAD0_;
    float _PAD1_;
    vec3  sphereCenter;
    float sphereRadius;
};

struct Fog {
    vec4  color;
    float startDistance;
    float endDistance;
    float transitionExponent;
    uint  enabled;
};

struct AtmosphereHaze {
    vec4  color;
    float density;
    uint  enabled;
    float _PAD0_;
    float _PAD1_;
};

struct AtmosphereLimbGlow {
    vec4  color;
    float intensity;
    uint  enabled;
    float _PAD0_;
    float _PAD1_;
};

struct Atmosphere {
    float              scaleHeight;
    uint               enabled;
    float              _PAD0_;
    float              _PAD1_;
    AtmosphereHaze     haze;
    AtmosphereLimbGlow limbGlow;
};

layout(std140) uniform EnvironmentBlock {
    uint defaultLightingEnabled;
    uint _pad0_0;
    uint _pad0_1;
    uint _pad0_2;

    uint         numAmbientLights;
    uint         _pad1_0;
    uint         _pad1_1;
    uint         _pad1_2;
    AmbientLight ambientLights[MAX_AMBIENT_LIGHTS];

    uint             numDirectionalLights;
    uint             _pad2_0;
    uint             _pad2_1;
    uint             _pad2_2;
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];

    uint       numPointLights;
    uint       _pad3_0;
    uint       _pad3_1;
    uint       _pad3_2;
    PointLight pointLights[MAX_POINT_LIGHTS];

    uint      numSpotLights;
    uint      _pad4_0;
    uint      _pad4_1;
    uint      _pad4_2;
    SpotLight spotLights[MAX_SPOT_LIGHTS];

    Fog fog;

    vec3  viewPosition_world;
    float _pad5_0;

    Surface    surface;
    Atmosphere atmosphere;
} Environment;

#endif // A3D_ENVIRONMENT_GLSL
