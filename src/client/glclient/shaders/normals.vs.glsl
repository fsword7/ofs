#version 430

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

out VS_OUT { vec3 normal; } vs;

uniform mat4 uWorld;

void main()
{
    gl_Position = uWorld * vec4(vPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(uWorld)));
    vs.normal = normalize(vec3(vec4(normalMatrix * vNormal, 0.0)));
}