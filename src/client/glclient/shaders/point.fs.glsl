#version 430

#include "logdepth.glsl"

uniform vec2  uCamClip;
// uniform float uCameraK;

in vec4 starColor;
out vec4 fragColor;

void main()
{
    float uCameraK = 1.0;

    // Rendering circular points
    vec2 circCoord = (gl_PointCoord * 2.0) - 1.0;
    if (dot(circCoord, circCoord) > 1.0)
        discard;

    fragColor = starColor;

    gl_FragDepth = getDepth(uCamClip.y, uCameraK);
}