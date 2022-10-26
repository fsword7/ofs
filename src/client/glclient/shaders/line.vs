#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec4 vColor;

uniform mat4 mvp;

out vec4 lineColor;

void main()
{
    gl_Position = mvp * vec4(vPosition, 1.0);
    lineColor = vColor;
}