#version 430

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

out VS_OUT { vec3 normal; } vs;

uniform mat4 uModel;
uniform mat4 uView;

void main()
{
    gl_Position = uView * uModel * vec4(vPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(uView * uModel)));
    vs.normal = normalize(vec3(vec4(normalMatrix * vNormal, 0.0)));
}