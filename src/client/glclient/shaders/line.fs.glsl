#version 430

uniform vec2  uCamClip;
// uniform float uCameraK;

#include "logdepth.glsl"

in vec4 lineColor;
out vec4 fragColor;

void main()
{
    float uCameraK = 1.0;

    fragColor = lineColor;

    gl_FragDepth = getDepth(uCamClip.y, uCameraK);
}