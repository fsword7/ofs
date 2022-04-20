#version 450

// vertex buffer objects
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec4 vColor;
layout (location = 2) in float vSize;

uniform mat4 mvp;

out vec4 starColor;

void main()
{
    gl_Position = mvp * vec4(vPosition, 1.0);
    gl_PointSize = vSize;
    starColor = vColor;
}