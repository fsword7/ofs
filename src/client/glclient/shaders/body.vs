#version 430

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 uViewProj;
// uniform mat4 uView;
uniform mat4 uModel;

// out vec4 eyePosition;
out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;

void main()
{
    normal = mat3(transpose(inverse(uModel))) * vNormal;
    fragPos = vec3(uModel * vec4(vPosition, 1.0));
    texCoord = vTexCoord;

    gl_Position = uViewProj * uModel * vec4(vPosition, 1.0);

    // eyePosition = uView * uModel * vec4(vPosition, 1.0);
}