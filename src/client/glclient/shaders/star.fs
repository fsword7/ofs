#version 430

// layout (binding = 0) uniform sampler2D sTile;

in vec3 fPosition;
in vec4 texCoord;
in vec3 normal;

out vec4 fragColor;

#include "snoise3.glsl"

void main()
{
    // Granules
    float n = (noise(normalize(fPosition), 4, 40.0, 0.7) + 1.0) * 0.5;
    // float total = n;

    float uRadius = 1.0;
    
    // Sunspots
    float s = 0.3;
    float freq = 0.00001;
    float t1 = snoise(fPosition * freq) - s;
    float t2 = snoise((fPosition + uRadius) * freq) - s;
    float ss = (max(t1, 0.0) * max(t2, 0.0)) * 2.0;
    // Total noise
    float total = n - ss;

    fragColor = vec4(total, total, total, 1.0);
    // fragColor = vec4(0.7, 0.7, 0.7, 1.0);
    // fragColor = texture(sTile, texCoord);
}