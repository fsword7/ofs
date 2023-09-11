#version 460

uniform float uTime;

#include "snoise3.glsl"
#include "lib/logdepth.glsl"

uniform vec2  uCamClip;
uniform float uCameraK;

int main()
{
    // vec3  nPosition = normalize(fPosition);

    // Star surface - granules
    vec3 pos = vec3(uTexCoords * 6.0, uTime * 0.003);
    float n = (noise(pos, 4, 40.0, 0.7) + 1.0) * 0.5;

    // Star surface - sunspots
    float s = 0.6;
    float uRadius = 100.0;
    vec3  spos = pos * uRadius;
    float freq = 0.02;
    float t1 = snoise(spos * freq) - s;
    float t2 = snoise((spos + uRadius) * freq) - s;
    float ss = (max(t1, 0.0) * max(t2, 0.0)) * 2.0;

    // Star surface - total noise (accumulate)
    float total = n - ss;

    fragColor = vec4((uColor.xyz * 0.5) + (total - 0.5), 1.0);

    gl_FragDepth = getDepth(uCamClip.y, uCameraK);
}