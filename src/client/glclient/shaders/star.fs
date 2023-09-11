#version 430

#include "logdepth.glsl"

// layout (binding = 0) uniform sampler2D sTile;

uniform vec4 uColor;
uniform vec3 uCentralDir;
uniform float uRadius;

uniform vec2  uCamClip;
// uniform float uCameraK;

in vec3 fPosition;
in vec4 texCoord;
in vec3 normal;
// in float vTime;

out vec4 fragColor;

#include "snoise3.glsl"

void main()
{
    float uCameraK = 1.0;
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

    gl_FragDepth = getDepth(uCamClip.y, uCameraK);
}