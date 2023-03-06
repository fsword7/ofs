#version 430

// layout (binding = 0) uniform sampler2D sTile;

uniform vec4 uColor;
uniform vec3 uCentralDir;
uniform float uRadius;

in vec3 fPosition;
in vec4 texCoord;
in vec3 normal;

out vec4 fragColor;

#include "snoise3.glsl"

void main()
{
    vec3  nPosition = normalize(fPosition);

    // Granules
    float n = (noise(nPosition, 4, 40.0, 0.7) + 1.0) * 0.5;

    // Sunspots
    float s = 0.36;
    float freq = 0.00001;
    float t1 = snoise(fPosition * freq) - s;
    float t2 = snoise((fPosition + uRadius) * freq) - s;
    float ss = (max(t1, 0.0) * max(t2, 0.0)) * 2.0;
    // Total noise
    float total = n - ss;

    // float theta = 1.0 - dot(uCentralDir, nPosition);

    fragColor = vec4((uColor.xyz * 0.5) + (total - 0.5), 1.0);
}