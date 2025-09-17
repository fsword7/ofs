#version 430

layout (location = 0) in vec3 vPositionh;
layout (location = 1) in vec3 vPositionl;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec2 vTexCoord;

uniform mat4 uViewProj;
// uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 urte;

uniform vec3 uCamEyeHigh;
uniform vec3 uCamEyeLow;

uniform float uTime;

// out vec4 eyePosition;
// out vec3 normal;
out vec3 fPosition;
out vec2 texCoord;
out float vTime;

void main()
{
    vTime = uTime;
    // vTime = 0.0;
    
    gl_Position = uViewProj * uModel * vec4(vPositionh, 1.0);
    texCoord = vTexCoord;
    // normal = mat3(transpose(inverse(uModel))) * vNormal;
    fPosition = vPositionh;

    // eyePosition = uViewProj * uModel * vec4(vPosition, 1.0);
}