#version 430

#include "logdepth.glsl"

uniform vec2  uCamClip;
// uniform float uCameraK;

layout (binding = 0) uniform sampler2D sTile;

// in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

void main()
{
    float uCameraK = 1.0;

    // fragColor = vec4(0.7, 0.7, 0.7, 1.0);
    fragColor = texture(sTile, texCoord);

    gl_FragDepth = getDepth(uCamClip.y, uCameraK);
}