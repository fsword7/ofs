#version 430

#include "logdepth.glsl"

#define MAX_NLIGHTS 10

struct lightSource
{
    vec3 spos;  // sun position
    vec3 diffuse;
    vec3 specular;

    // float attenuation[4];
};

uniform vec3  uAmbient;
uniform int   unLights;
uniform lightSource lights[MAX_NLIGHTS];

uniform vec2  uCamClip;
// uniform float uCameraK;

layout (binding = 0) uniform sampler2D sTile;

// layout (std140) uniform Lights {
//     lightSource light[MAX_NLIGHTS];
//     int nLights;
// };

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

out vec4 fragColor;

void addLight(in lightSource light, in vec3 normal, in vec3 fragPos, in vec3 viewDir,
    in float specPower, inout vec3 diffuse, inout vec3 specular)
{
    vec3 lightDir = normalize(light.spos - fragPos);

    float diff = clamp(dot(-normal, lightDir), 0.0, 1.0);
    float spec = 0.0;

    // vec3 reflectDir = reflect(-lightDir, normal);
    // if (specPower != 0 && diff > 0)
    //     spec = pow(clamp(dot(viewDir, reflectDir), 0.0, 1.0), specPower);

    diffuse += light.diffuse * diff;
    specular += light.diffuse * spec;
}

void main()
{
    float uCameraK = 1.0;


    fragColor = texture(sTile, texCoord);

    vec3 diff = uAmbient;
    vec3 spec = vec3(0.0);
    vec3 norm = normalize(normal);
    vec3 vdir = normalize(-fragPos);

    for (int idx = 0; idx < unLights; idx++)
        addLight(lights[idx], norm, fragPos, vdir,
            0.0, diff, spec);

    fragColor.rgb *= diff;

    gl_FragDepth = getDepth(uCamClip.y, uCameraK);
}