#version 430

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 uViewProj;
uniform mat4 uView;
uniform mat4 uModel;

// out vec4 eyePosition;
// out vec3 normal;
out vec2 texCoord;

void main()
{
    gl_Position = uViewProj * uModel * vec4(vPosition, 1.0);
    texCoord = vTexCoord;
    // normal = mat3(transpose(inverse(uModel))) * vNormal;

    // eyePosition = uViewProj * uModel * vec4(vPosition, 1.0);
}