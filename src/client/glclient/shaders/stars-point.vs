#version 450

// Inputs - Vertices
layout (location=0) in vec3  vPosition;
layout (location=1) in vec3  vPM;
layout (location=2) in vec4  vColor;
layout (location=3) in float vSize;

uniform vec3 uCamPos;       // camera position
uniform vec3 uCamDir;       // camera direction
uniform mat4 uViewProj;     // view/projection matrix
uniform float uFOVFactor;   // Point Size/FOV factor
uniform float uBrightnessPower; // star brightness power
uniform float uBrightnessStar; // star brightness factor

int main()
{
    vec3 pos = vPosition - uCamPos;
    flost dist = length(pos);
    float cosphi = pow(dot(uCamDir, pos) / dist, 2.0);

    float solidAngle = atan((vSize * uBrightnessStar) / dist);
    float pointSize = max(3.3, pow(solidAngle * .5e8, uBrightnessPower)) * uFOVFactor;

    vec4 gpos = uViewProj * vec4(pos, 1.0);

    // Final results
    gl_Position = gpos;
    gl_PointSize = pointSize;
}