#version 430

#include "logdepth.glsl"

uniform vec2 uCamClip;
uniform vec4 uColor;

out vec4 fragColor;

void main()
{
    float uCameraK = 1.0;

    fragColor = uColor;
    gl_FragDepth = getDepth(uCamClip.y, uCameraK);
}