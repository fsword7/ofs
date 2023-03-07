#version 430

uniform vec4  uColor;
uniform float uMaxSize;
uniform float uDT;

in vec3 fPosition;

out vec4 fragColor;

#include "snoise4.glsl"

void main()
{
    const float brightnessMultipler = 0.0;
    const float smoothingMultipler = 0.15;
    const float ringIntesityMultipler = 2.8;
    const float coronaSizeMultipler = 2.0;
    const float frequency = 1.5;
    const float fDetail = 0.7;
    const int iDetail = 10;
    const float irregularityMultipler = 4;

    float t = uDT * 10.0 - length(fPosition);

    // offset normal with noise
    float ox = snoise(vec4(fPosition, t) * frequency);
    float oy = snoise(vec4((fPosition + (1000.0 * irregularityMultipler)), t) * frequency);
    float oz = snoise(vec4((fPosition + (2000.0 * irregularityMultipler)), t) * frequency);
    float om = snoise(vec4((fPosition + (4000.0 * irregularityMultipler)), t) * frequency) *
               snoise(vec4((fPosition + (250.0  * irregularityMultipler)), t) * frequency);

    vec3 offsetVec = vec3(ox * om, oy * om, oz * om) * smoothingMultipler;

    vec3 nDistVec = normalize(fPosition + offsetVec);

    vec3 position = fPosition + noise(vec4(nDistVec, t), iDetail, 1.5, fDetail) * smoothingMultipler;

    float dist = length(position + offsetVec) * coronaSizeMultipler;
    float brightness1 = (1.0 / (dist * dist) - 0.1) * (brightnessMultipler - 0.4);
    float brightness2 = (1.0 / (dist * dist)) * brightnessMultipler;

    vec3 color = uColor.rgb * brightness1;

    fragColor = vec4(color, clamp(brightness1, 0.0, 1.0) * (cos(clamp(brightness1, 0.0, 0.5)) / 
            (cos(clamp(brightness2 / ringIntesityMultipler, 0.0, 1.5)) * 2)));
}