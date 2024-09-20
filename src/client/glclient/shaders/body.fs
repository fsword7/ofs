#version 430

#include "logdepth.glsl"

#define MAX_NLIGHTS 10

struct lightSource
{
    bool enabled;
    vec3 spos;  // sun position
    vec3 diffuse;
    vec3 specular;

    // float attenuation[4];
};

uniform lightSource lights[MAX_NLIGHTS];
uniform vec3  uAmbient;
uniform int   unLights;

uniform vec2  uCamClip;
// uniform float uCameraK;

layout (binding = 0) uniform sampler2D sTile;

// layout (std140) uniform Lights {
//     lightSource light[MAX_NLIGHTS];
//     int nLights;
// };

in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

void addLight(in lightSource light, in vec3 normal, in vec3 fragPos, in vec3 viewDir,
    in float specPower, inout vec4 diffuse, inout vec4 specular)
{
    if (!light.enabled)
        return;

    vec3 lightDir = normalize(light.spos - fragPos);

    float diff = clamp(dot(normal, lightDir), 0.0, 1.0);
    float spec = 0.0;

    vec3 reflectDir = reflect(-lightDir, normal);
    if (specPower != 0 && diff > 0)
        spec = pow(clamp(dot(viewDir, reflectDir), 0.0, 1.0), specPower);

    diffuse += light.diffuse.rgb * diff;
    specular += light.diffuse.rgb * spec;
}

void main()
{
    float uCameraK = 1.0;

    vec3 diff = uAmbient;
    vec3 spec = vec3(0.0);
    // for (int idx = 0; idx < unLights; idx++)
    //     addLight(Lights.light[idx], normal, fragPos, viewDir,
    //         specPower, diff, spec);

    // fragColor = vec4(0.7, 0.7, 0.7, 1.0);
    fragColor = texture(sTile, texCoord);

    gl_FragDepth = getDepth(uCamClip.y, uCameraK);
}